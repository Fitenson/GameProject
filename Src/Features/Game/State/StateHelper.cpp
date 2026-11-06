#include "StateHelper.h"
#include "../../Character/Weapon/WeaponFactory.h"
#include "../../Character/NPC/Trooper.h"
#include "../../Character/Avatar/Assasin.h"

void StateHelper::drawAmmo(GameContext& context)
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

void StateHelper::renderGameScene(GameContext& context)
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

void StateHelper::drawHud(GameContext& context)
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

// Trees, rocks, walls, gates and cages block movement. Slide along one axis if possible.
void StateHelper::keepOutOfObstacles(const Map& map, Entity& entity, const Vector2& before)
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

UniquePtr<Ally> StateHelper::makeTrooperAlly(const Vector2& position)
{
    Character character("Trooper", 1, makeUnique<Trooper>());
    character.equip(WeaponFactory::create(WeaponType::Gun));
    return makeUnique<Ally>(std::move(character), position);
}

UniquePtr<Ally> StateHelper::makeShadowClone(const Vector2& position, float lifetime)
{
    Character character("Shadow", 1, makeUnique<Assasin>());
    character.equip(WeaponFactory::create(WeaponType::Sword));
    character.getDamage() *= 0.6f;   // the clone hits softer than the real Assasin

    UniquePtr<Ally> clone = makeUnique<Ally>(std::move(character), position);
    clone->makeTemporary(lifetime);
    clone->setCombatStyle(52.0f, 260.0f, 72.0f);
    return clone;
}

int StateHelper::permanentAllies(const std::vector<UniquePtr<Ally>>& allies)
{
    return static_cast<int>(std::count_if(allies.begin(), allies.end(),
        [](const UniquePtr<Ally>& ally) { return !ally->isTemporary(); }));
}