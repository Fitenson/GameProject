#include <algorithm>
#include <cmath>
#include <string>
#include "Paused.h"
#include "Engine/Graphics.h"

void GameState::Paused::enter(GameFSM::Control& control)
{
    GameContext& context = control.context();

    context.dialog = makeUnique<Engine::UI::Dialog>("PAUSED");
    context.dialog->addButton("RESUME");
    context.dialog->addButton("ARMORY");
    context.dialog->addButton("RESTART");
    context.dialog->addButton("MAIN MENU");
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
    case 0:
        control.changeTo<GameState::Playing>();
        break;

    case 1:
        control.changeTo<GameState::Armory>();
        break;

    case 2:
        context.allies.clear();
        context.player->getCoins() = 0;
        control.changeTo<GameState::Loading>();
        break;

    case 3:
        control.changeTo<GameState::MainMenu>();
        break;

    default:
        break;
    }
}

void GameState::Paused::react(const RenderEvent&, GameFSM::EventControl& control)
{
    GameContext& context = control.context();

    if (!context.world || !context.player) return;

    Engine::Graphics::Renderer& renderer = context.renderer;

    // World rendering
    renderer.beginWorld(context.world->getCamera().getCamera());
    context.world->drawChunks(renderer);
    context.world->drawEntities(renderer);

    for (const UniquePtr<Ally>& ally : context.allies) {
        ally->render(renderer);
    }

    context.player->render(renderer);
    renderer.endWorld();

    const Stage& stage = context.world->getStage();
    const Map& map = context.world->getMap();

    // Stage and wave
    const std::string stageText = "STAGE " + std::to_string(stage.getNumber());

    const std::string waveText = stage.isResting()
        ? "WAVE " + std::to_string(stage.getWave() + 1) +
        " IN " + std::to_string(static_cast<int>(std::ceil(stage.getRestSeconds())))
        : "WAVE " + std::to_string(stage.getWave()) +
        "/" + std::to_string(stage.getWaveCount());

    renderer.drawText(stageText + "   " + waveText,
        Vector2{ 16.0f, 16.0f }, 22, WHITE);

    // Kill progress
    const float ratio = std::clamp(
        static_cast<float>(stage.getKills()) /
        static_cast<float>(std::max(1, stage.getKillTarget())),
        0.0f, 1.0f);

    const Rectangle bar{ 16.0f, 46.0f, 240.0f, 12.0f };

    renderer.drawRectangle(bar, Color{ 20, 20, 20, 200 });

    renderer.drawRectangle(
        Rectangle{ bar.x, bar.y, bar.width * ratio, bar.height },
        Color{ 230, 90, 70, 255 });

    renderer.drawRectangleOutline(bar, 1.0f, BLACK);

    renderer.drawText(
        std::to_string(stage.getKills()) + "/" +
        std::to_string(stage.getKillTarget()) + " defeated",
        Vector2{ 266.0f, 43.0f }, 18, WHITE);

    // Puzzle objective, hints and keys
    float y = 68.0f;

    renderer.drawText(map.getObjective(),
        Vector2{ 16.0f, y }, 20, WHITE);

    y += 26.0f;

    if (stage.isWaitingForGates()) {
        renderer.drawText(
            "Open every gate to finish the stage!",
            Vector2{ 16.0f, y }, 20,
            Color{ 255, 170, 80, 255 });

        y += 26.0f;
    }

    if (map.getKeysHeld() > 0) {
        renderer.drawText(
            "Keys: " + std::to_string(map.getKeysHeld()),
            Vector2{ 16.0f, y }, 20,
            Color{ 245, 200, 60, 255 });

        y += 26.0f;
    }

    const std::string prompt =
        map.getPrompt(context.player->getPosition());

    if (!prompt.empty()) {
        renderer.drawText(prompt,
            Vector2{ 16.0f, y }, 20,
            Color{ 255, 230, 120, 255 });
    }

    // Scrap
    renderer.drawText(
        "Scrap: " + std::to_string(context.player->getCoins()),
        Vector2{ 16.0f, static_cast<float>(GetScreenHeight()) - 110.0f },
        20, Color{ 245, 200, 60, 255 });

    // Ammunition HUD
    const AmmoInfo ammo =
        context.player->getCharacter().getCurrentAmmoInfo();

    if (ammo.used) {
        const float ammoY =
            static_cast<float>(GetScreenHeight()) - 76.0f;

        if (ammo.reloading) {
            renderer.drawText("RELOADING...",
                Vector2{ 16.0f, ammoY }, 26,
                Color{ 245, 200, 60, 255 });
        }
        else {
            const bool low =
                ammo.loaded <= std::max(1, ammo.capacity / 4);

            renderer.drawText(
                std::to_string(ammo.loaded) + " / " +
                std::to_string(ammo.capacity),
                Vector2{ 16.0f, ammoY }, 30,
                low ? Color{ 230, 90, 70, 255 } : WHITE);
        }

        renderer.drawText(
            "R: reload   (spare bullets: " +
            std::to_string(ammo.standby) + ")",
            Vector2{ 16.0f, ammoY + 34.0f },
            14, Color{ 200, 215, 215, 255 });
    }

    // Notification
    if (context.noticeTimer > 0.0f) {
        const int w = renderer.measureText(context.notice, 24);

        renderer.drawText(context.notice,
            Vector2{
                (static_cast<float>(GetScreenWidth()) - w) * 0.5f,
                120.0f
            },
            24, Color{ 255, 230, 120, 255 });
    }

    // Pause dialog
    if (context.dialog) {
        context.dialog->render(renderer);
    }
}
