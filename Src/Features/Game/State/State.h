#pragma once

#include <hfsm2/machine.hpp>
#include "../GameContext.h"

class RenderEvent {};

using GameConfig = hfsm2::Config::ContextT<GameContext&>;
using GameMachine = hfsm2::MachineT<GameConfig>;

namespace GameState {
    class MainMenu;
    class Loading;
    class Playing;
    class Paused;
    class Armory;
    class StageComplete;
    class GameOver;
}

using GameFSM = GameMachine::PeerRoot<
    GameState::MainMenu,
    GameState::Loading,
    GameState::Playing,
    GameState::Paused,
    GameState::Armory,
    GameState::StageComplete,
    GameState::GameOver
>;
