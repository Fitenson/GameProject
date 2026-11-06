#include "ArmoryMenu.h"
#include "../../Menu/ArmoryMenu.h"

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
