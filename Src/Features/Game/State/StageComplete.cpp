#include "StageComplete.h"
#include "StateHelper.h"

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
    StateHelper::renderGameScene(context);
    if (context.dialog) context.dialog->render(context.renderer);
}
