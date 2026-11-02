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
#include "../Menu/ArmoryMenu.h"
#include "GameState.h"
#include "../Character/Avatar/AvatarFactory.h"
#include "../Character/Weapon/WeaponFactory.h"

namespace {
    constexpr int CoinPerKill = 5;

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

    void drawAmmo(GameContext& context)
    {
        Engine::Graphics::Renderer& renderer = context.renderer;
        const AmmoInfo ammo = context.player->getCharacter().getCurrentAmmoInfo();
        if (!ammo.used) return;   // swords, guns without a magazine: nothing to show

        const float y = static_cast<float>(GetScreenHeight()) - 76.0f;

        if (ammo.reloading) {
            renderer.drawText("RELOADING...", Vector2{ 16.0f, y }, 26, Color{ 245, 200, 60, 255 });
        }
        else {
            const bool low = ammo.loaded <= std::max(1, ammo.capacity / 4);
            renderer.drawText(std::to_string(ammo.loaded) + " / " + std::to_string(ammo.capacity),
                Vector2{ 16.0f, y }, 30, low ? Color{ 230, 90, 70, 255 } : WHITE);
        }

        renderer.drawText("R: reload   (spare bullets: " + std::to_string(ammo.standby) + ")",
            Vector2{ 16.0f, y + 34.0f }, 14, Color{ 200, 215, 215, 255 });
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

        renderer.drawText("Scrap: " + std::to_string(context.player->getCoins()),
            Vector2{ 16.0f, static_cast<float>(GetScreenHeight()) - 110.0f }, 20, Color{ 245, 200, 60, 255 });

        drawAmmo(context);

        if (context.noticeTimer > 0.0f) {
            const int w = renderer.measureText(context.notice, 24);
            renderer.drawText(context.notice,
                Vector2{ (static_cast<float>(GetScreenWidth()) - w) * 0.5f, 120.0f }, 24, Color{ 255, 230, 120, 255 });
        }
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

	Player* player = context.player.get();

    context.dialog.reset();
    context.allies.clear();
    context.player.reset();
    context.world.reset();

    if (player != nullptr) {
        player->getCoins() = 0;
    }

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

    Character character;
    if (context.keepCharacter && context.player) {
        character = std::move(context.player->getCharacter());   // keeps upgrades and attachments
        character.heal(character.getMaxHp());
    }
    else {
        character = Character("User001", 1, AvatarFactory::createAvatar(context.selectedAvatar));
        character.equip(WeaponFactory::create(startingWeapon(context.selectedAvatar)));

        const float shieldRatio = character.getAvatar().getShieldRatio();
        if (shieldRatio > 0.0f) {
            character.equip(makeUnique<Shield>(character.getMaxHp() * shieldRatio));
        }
    }
    context.keepCharacter = false;

    context.player = makeUnique<Player>(std::move(character), spawn);
    context.world->setFocus(spawn);

    // Every stage counts its kills from zero. Scrap carries over.
    context.lastKills = 0;

    // Allies from the previous stage start next to the new spawn point.
    float offset = 0.0f;
    for (UniquePtr<Ally>& ally : context.allies) {
        offset += 24.0f;
        ally->setPosition(Vector2{ spawn.x - offset, spawn.y + 10.0f });
        ally->setLeader(spawn);
    }

    std::vector<Target> noTargets;
    context.world->update(0.0f, context.player->getHitPosition(), noTargets);

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
            if (context.allies.size() < 3) {   // cap the party size
                context.allies.push_back(makeTrooperAlly(map.getPrisonerPosition()));
            }
        }
    }

    if (IsKeyPressed(KEY_Q)) {
        context.player->getCharacter().activateAbility();
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

    for (const Item& item :
        context.world->collectItems(context.player->getPosition()))
    {
        const Attachment& attachment = item.attachment;

        std::visit(
            [&](const auto& value)
            {
                using T = std::decay_t<decltype(value)>;

                if constexpr (std::is_same_v<T, GunAttachment>)
                {
                    ++context.attachmentStock[indexOf(value)];
                }
                else if constexpr (std::is_same_v<T, SwordAttachment>)
                {
                    ++context.attachmentStock[indexOf(value)];
                }
            },
            attachment.attachment
        );

        context.notice = "Found: "
            + Weapon::getAttachmentName(attachment)
            + "  (install it in the Armory)";

        context.noticeTimer = 3.0f;
    }


    context.noticeTimer = std::max(0.0f, context.noticeTimer - context.deltaTime);

    // Coins for every enemy defeated since the last frame.
    const int kills = context.world->getStage().getKills();
    context.player->getCoins() = context.player->getCoins() + (kills - context.lastKills) * CoinPerKill;
    context.lastKills = kills;

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
    context.dialog->addButton("ARMORY");     // 1
    context.dialog->addButton("RESTART");    // 2
    context.dialog->addButton("MAIN MENU");  // 3
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
    case 1: control.changeTo<GameState::Armory>();   break;
    case 2:   // restart begins a new run: no party, no coins
        context.allies.clear();
        context.player->getCoins() = 0;
        control.changeTo<GameState::Loading>();
        break;
    case 3: control.changeTo<GameState::MainMenu>(); break;
    default: break;
    }
}

void GameState::Paused::react(const RenderEvent&, GameFSM::EventControl& control)
{
    GameContext& context = control.context();
    renderGameScene(context);
    if (context.dialog) context.dialog->render(context.renderer);
}

// ---------- Armory ----------

void GameState::Armory::enter(GameFSM::Control& control)
{
    GameContext& context = control.context();

    context.dialog.reset();
    context.navigation.clear();

    if (context.player) {
        context.navigation.push(makeUnique<ArmoryMenu>(
            context.player->getCharacter(), context.allies, context.player->getCoins(), context.attachmentStock,
            context.armoryFromStage ? "NEXT STAGE" : "BACK"));
    }
}

void GameState::Armory::update(GameFSM::FullControl& control)
{
    GameContext& context = control.context();

    context.navigation.update(context.deltaTime);

    if (!context.navigation.isEmpty()) return;

    if (context.armoryFromStage) {                 // upgrades done: start the next stage
        context.armoryFromStage = false;
        context.keepCharacter = true;
        ++context.selectedStage;
        control.changeTo<GameState::Loading>();
    }
    else {
        control.changeTo<GameState::Paused>();
    }
}


void GameState::Armory::react(
    const RenderEvent&,
    GameFSM::EventControl& control
)
{
    GameContext& context = control.context();
    context.navigation.render(context.renderer);
}


// ---------- GameOver ----------

void GameState::GameOver::enter(GameFSM::Control& control)
{
    GameContext& context = control.context();

    // The run is over: the party and the scrap are lost.
    context.allies.clear();
    context.player->getCoins() = 0;

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
        context.armoryFromStage = true;
        control.changeTo<GameState::Armory>();
        break;
    case 1:   // replay: same stage, keep the character and its upgrades
        context.keepCharacter = true;
        control.changeTo<GameState::Loading>();
        break;
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