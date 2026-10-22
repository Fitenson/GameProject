#include <algorithm>
#include <cmath>
#include <string>
#include <utility>
#include <vector>
#include <raylib.h>
#include "Engine/Core.h"
#include "../Character/NPC/Trooper.h"
#include "../Menu/MainMenu.h"
#include "../Menu/StageScreen.h"
#include "GameState.h"
#include "../Character/Avatar/AvatarFactory.h"
#include "../Character/Weapon/WeaponFactory.h"

namespace {
    // Trees, rocks, walls, gates and cages block movement. Slide along one axis if possible.
    void keepOutOfObstacles(const Map& map, Entity& entity, const Vector2& before)
    {
        Vector2 after = entity.getPosition();
        if (!map.isBlocked(after)) return;

        const Vector2 slideX{ after.x, before.y };
        const Vector2 slideY{ before.x, after.y };

        if (!map.isBlocked(slideX))      after = slideX;
        else if (!map.isBlocked(slideY)) after = slideY;
        else                             after = before;

        entity.setPosition(after);
    }

    UniquePtr<Ally> makeTrooperAlly(const Vector2& position)
    {
        Character character("Trooper", 1, makeUnique<Trooper>());
        character.equip(WeaponFactory::create(WeaponType::Gun));
        return makeUnique<Ally>(std::move(character), position);
    }

    WeaponType startingWeapon(AvatarVariant variant)
    {
        switch (variant) {
        case AvatarVariant::Assasin: return WeaponType::Sword;
        case AvatarVariant::Soldier: return WeaponType::Gun;
        case AvatarVariant::Tanker:  return WeaponType::HeavyGun;
        }
        return WeaponType::Gun;
    }

    void drawHud(GameContext& context)
    {
        Engine::Graphics::Renderer& renderer = context.renderer;
        const Stage& stage = context.world->getStage();
        const Map& map = context.world->getMap();

        // Stage and wave
        const std::string stageText = "STAGE " + std::to_string(stage.getNumber());
        const std::string waveText = stage.isResting()
            ? "WAVE " + std::to_string(stage.getWave() + 1) + " IN " + std::to_string(static_cast<int>(std::ceil(stage.getRestSeconds())))
            : "WAVE " + std::to_string(stage.getWave()) + "/" + std::to_string(stage.getWaveCount());
        renderer.drawText(stageText + "   " + waveText, Vector2{ 16.0f, 16.0f }, 22, WHITE);

        // Kill progress
        const float ratio = std::clamp(static_cast<float>(stage.getKills()) / static_cast<float>(std::max(1, stage.getKillTarget())), 0.0f, 1.0f);
        const Rectangle bar{ 16.0f, 46.0f, 240.0f, 12.0f };
        renderer.drawRectangle(bar, Color{ 20, 20, 20, 200 });
        renderer.drawRectangle(Rectangle{ bar.x, bar.y, bar.width * ratio, bar.height }, Color{ 230, 90, 70, 255 });
        renderer.drawRectangleOutline(bar, 1.0f, BLACK);
        renderer.drawText(std::to_string(stage.getKills()) + "/" + std::to_string(stage.getKillTarget()) + " defeated",
            Vector2{ 266.0f, 43.0f }, 18, WHITE);

        // Puzzle objective, hints and keys
        float y = 68.0f;
        renderer.drawText(map.getObjective(), Vector2{ 16.0f, y }, 20, WHITE);
        y += 26.0f;

        if (stage.isWaitingForGates()) {
            renderer.drawText("Open every gate to finish the stage!", Vector2{ 16.0f, y }, 20, Color{ 255, 170, 80, 255 });
            y += 26.0f;
        }
        if (map.getKeysHeld() > 0) {
            renderer.drawText("Keys: " + std::to_string(map.getKeysHeld()), Vector2{ 16.0f, y }, 20, Color{ 245, 200, 60, 255 });
            y += 26.0f;
        }

        const std::string prompt = map.getPrompt(context.player->getPosition());
        if (!prompt.empty()) renderer.drawText(prompt, Vector2{ 16.0f, y }, 20, Color{ 255, 230, 120, 255 });
    }

    // The world, allies, player and HUD. Used by Playing, Paused, GameOver and StageComplete.
    void renderGameScene(GameContext& context)
    {
        if (!context.world || !context.player) return;

        Engine::Graphics::Renderer& renderer = context.renderer;

        renderer.beginWorld(context.world->getCamera().getCamera());
        context.world->drawChunks(renderer);
        context.world->drawEntities(renderer);
        for (const UniquePtr<Ally>& ally : context.allies) ally->render(renderer);
        context.player->render(renderer);
        renderer.endWorld();

        drawHud(context);
    }
}

// ---------- MainMenu ----------

void GameState::MainMenu::enter(GameFSM::Control& control)
{
    GameContext& context = control.context();

    context.dialog.reset();
    context.allies.clear();
    context.player.reset();
    context.world.reset();

    context.startRequested = false;
    context.navigation.clear();

    // ::MainMenu is the screen class; inside this namespace the plain name means the state.
    context.navigation.push(makeUnique<::MainMenu>(
        [&context](AvatarVariant variant) {
            // Avatar confirmed: the stage screen goes on top of the stack.
            context.selectedAvatar = variant;
            context.navigation.push(makeUnique<StageScreen>(
                context.unlockedStages,
                [&context](int stage) {
                    context.selectedStage = stage;
                    context.startRequested = true;   // the state reacts in update()
                }
            ));
        },
        [&context]() { context.app.stop(); }
    ));
}

void GameState::MainMenu::update(GameFSM::FullControl& control)
{
    GameContext& context = control.context();

    context.navigation.update(context.deltaTime);

    if (context.startRequested) {
        context.startRequested = false;
        context.navigation.clear();
        control.changeTo<GameState::Loading>();
    }
}

void GameState::MainMenu::react(const RenderEvent&, GameFSM::EventControl& control)
{
    GameContext& context = control.context();
    context.navigation.render(context.renderer);
}

// ---------- Loading ----------

void GameState::Loading::update(GameFSM::FullControl& control)
{
    GameContext& context = control.context();

    context.dialog.reset();

    // The stage number picks the seed, so the same stage always gives the same map.
    context.world = makeUnique<World>(StageConfig::forStage(context.selectedStage));
    const Vector2 spawn = context.world->getMap().getSpawn();

    Character character(
        "User001",
        1,
        AvatarFactory::createAvatar(context.selectedAvatar)
    );

    character.equip(WeaponFactory::create(startingWeapon(context.selectedAvatar)));

    const float shieldRatio = character.getAvatar().getShieldRatio();
    if (shieldRatio > 0.0f) {
        character.equipShield(makeUnique<Shield>(character.getMaxHp() * shieldRatio));
    }

    context.player = makeUnique<Player>(std::move(character), spawn);
    context.world->setFocus(spawn);

    std::vector<Target> noTargets;
    context.world->update(0.0f, context.player->getHitPosition(), noTargets);
    context.allies.clear();

    control.changeTo<GameState::Playing>();
}

// ---------- Playing ----------

void GameState::Playing::update(GameFSM::FullControl& control)
{
    GameContext& context = control.context();
    if (!context.world || !context.player) return;

    if (IsKeyPressed(KEY_ESCAPE)) {
        control.changeTo<GameState::Paused>();
        return;
    }

    Map& map = context.world->getMap();

    const Vector2 mouseWorld = context.world->getCamera().screenToWorld2D(GetMousePosition());
    context.player->setAimTarget(mouseWorld);

    const Vector2 before = context.player->getPosition();
    context.player->update(context.deltaTime);
    keepOutOfObstacles(map, *context.player, before);

    // E: use a lever, key, switch, rune or tablet or a locked door. Otherwise free the prisoner.
    if (IsKeyPressed(KEY_E)) {
        const Vector2 p = context.player->getPosition();
        if (!map.interact(p) && map.freePrisoner(p)) {
            context.allies.push_back(makeTrooperAlly(map.getPrisonerPosition()));
        }
    }

    for (UniquePtr<Ally>& ally : context.allies) {
        const Vector2 allyBefore = ally->getPosition();
        ally->setLeader(context.player->getPosition());
        ally->update(context.deltaTime);
        keepOutOfObstacles(map, *ally, allyBefore);
    }

    // Enemy targets: index 0 is the player, index 1+i is allies[i].
    std::vector<Target> targets;
    targets.push_back({ context.player->getHitPosition(), context.player->getHitRadius(), !context.player->isDead() });
    for (const UniquePtr<Ally>& ally : context.allies) {
        targets.push_back({ ally->getHitPosition(), ally->getHitRadius(), !ally->isDead() });
    }

    context.world->update(context.deltaTime, context.player->getPosition(), targets);

    if (targets[0].damage > 0.0f) context.player->takeDamage(targets[0].damage);
    for (size_t i = 0; i < context.allies.size(); ++i) {
        if (targets[i + 1].damage > 0.0f) context.allies[i]->takeDamage(targets[i + 1].damage);
    }

    if (context.player->isDead()) {
        control.changeTo<GameState::GameOver>();
        return;
    }

    // Enough enemies defeated: the stage is complete and the next one unlocks.
    if (context.world->getStage().isComplete()) {
        context.unlockedStages = std::max(context.unlockedStages, std::min(StageConfig::Count, context.selectedStage + 1));
        control.changeTo<GameState::StageComplete>();
        return;
    }

    std::erase_if(context.allies, [](const UniquePtr<Ally>& ally) { return ally->isDead(); });

    std::vector<Enemy*> enemies;
    context.world->collectTargets(enemies);
    context.player->updateWeapons(context.deltaTime, enemies);

    for (UniquePtr<Ally>& ally : context.allies) {
        ally->updateCombat(context.deltaTime, enemies);
    }
}

void GameState::Playing::react(const RenderEvent&, GameFSM::EventControl& control)
{
    renderGameScene(control.context());
}

// ---------- Paused ----------

void GameState::Paused::enter(GameFSM::Control& control)
{
    GameContext& context = control.context();

    context.dialog = makeUnique<Engine::UI::Dialog>("PAUSED");
    context.dialog->addButton("RESUME");     // 0
    context.dialog->addButton("RESTART");    // 1
    context.dialog->addButton("MAIN MENU");  // 2
}

void GameState::Paused::update(GameFSM::FullControl& control)
{
    GameContext& context = control.context();
    if (!context.dialog) return;

    if (IsKeyPressed(KEY_ESCAPE)) {
        control.changeTo<GameState::Playing>();
        return;
    }

    switch (context.dialog->update(context.deltaTime)) {
    case 0: control.changeTo<GameState::Playing>();  break;
    case 1: control.changeTo<GameState::Loading>();  break;   // restart the same stage
    case 2: control.changeTo<GameState::MainMenu>(); break;
    default: break;
    }
}

void GameState::Paused::react(const RenderEvent&, GameFSM::EventControl& control)
{
    GameContext& context = control.context();
    renderGameScene(context);
    if (context.dialog) context.dialog->render(context.renderer);
}

// ---------- GameOver ----------

void GameState::GameOver::enter(GameFSM::Control& control)
{
    GameContext& context = control.context();

    context.dialog = makeUnique<Engine::UI::Dialog>("GAME OVER", "You were overwhelmed.");
    context.dialog->addButton("RESTART");    // 0
    context.dialog->addButton("MAIN MENU");  // 1
}

void GameState::GameOver::update(GameFSM::FullControl& control)
{
    GameContext& context = control.context();
    if (!context.dialog) return;

    switch (context.dialog->update(context.deltaTime)) {
    case 0: control.changeTo<GameState::Loading>();  break;
    case 1: control.changeTo<GameState::MainMenu>(); break;
    default: break;
    }
}

void GameState::GameOver::react(const RenderEvent&, GameFSM::EventControl& control)
{
    GameContext& context = control.context();
    renderGameScene(context);
    if (context.dialog) context.dialog->render(context.renderer);
}

// ---------- StageComplete ----------

void GameState::StageComplete::enter(GameFSM::Control& control)
{
    GameContext& context = control.context();

    const bool last = context.selectedStage >= StageConfig::Count;
    const int kills = context.world ? context.world->getStage().getKills() : 0;

    context.dialog = makeUnique<Engine::UI::Dialog>(
        "STAGE " + std::to_string(context.selectedStage) + " COMPLETE",
        last ? "You cleared every stage!" : "Enemies defeated: " + std::to_string(kills));

    if (!last) context.dialog->addButton("NEXT STAGE");   // 0
    context.dialog->addButton("REPLAY");                  // 1 (0 on the last stage)
    context.dialog->addButton("MAIN MENU");               // 2 (1 on the last stage)
}

void GameState::StageComplete::update(GameFSM::FullControl& control)
{
    GameContext& context = control.context();
    if (!context.dialog) return;

    int choice = context.dialog->update(context.deltaTime);
    if (choice < 0) return;

    // Without the NEXT button the indices shift down by one, so shift them back.
    if (context.selectedStage >= StageConfig::Count) ++choice;

    switch (choice) {
    case 0:
        ++context.selectedStage;
        control.changeTo<GameState::Loading>();
        break;
    case 1: control.changeTo<GameState::Loading>();  break;
    case 2: control.changeTo<GameState::MainMenu>(); break;
    default: break;
    }
}

void GameState::StageComplete::react(const RenderEvent&, GameFSM::EventControl& control)
{
    GameContext& context = control.context();
    renderGameScene(context);
    if (context.dialog) context.dialog->render(context.renderer);
}