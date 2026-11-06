#include "MainMenu.h"
#include "../../Menu/MainMenu.h"
#include "../../Menu/StageScreen.h"

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