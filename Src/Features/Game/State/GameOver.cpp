#include "GameOver.h"
#include "StateHelper.h"

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
    StateHelper::renderGameScene(context);
    if (context.dialog) context.dialog->render(context.renderer);
}