#include <raylib.h>
#include "Engine/Core.h"
#include "Src/Features/Menu/MainMenu.h"
#include "Src/Features/Game/Game.h"

int main()
{
    Engine::ApplicationConfig config {
        .width = 1280,
        .height = 720,
        .title = "Attack",
        .targetFPS = 60,
        .exitOnEscape = false
    };

    Engine::Application app(config);
    Game game(app);

    app.run([&](float deltaTime) {
        game.update(deltaTime);
        game.render();
    });

    return 0;
}