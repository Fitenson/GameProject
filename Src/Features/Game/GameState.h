#pragma once

#include <hfsm2/machine.hpp>
#include "GameContext.h"

class RenderEvent {};

using GameConfig = hfsm2::Config::ContextT<GameContext&>;
using GameMachine = hfsm2::MachineT<GameConfig>;

namespace GameState {
    class MainMenu;
    class Loading;
    class Playing;
    class Paused;
    class StageComplete;
    class GameOver;
}

// MainMenu is first, so it is the initial state.
using GameFSM = GameMachine::PeerRoot<
    GameState::MainMenu,
    GameState::Loading,
    GameState::Playing,
    GameState::Paused,
    GameState::StageComplete,
    GameState::GameOver
>;

namespace GameState {
    class MainMenu : public GameFSM::State {
    public:
        void enter(GameFSM::Control& control);
        void update(GameFSM::FullControl& control);
        void react(const RenderEvent& event, GameFSM::EventControl& control);
    };

    class Loading : public GameFSM::State {
    public:
        void update(GameFSM::FullControl& control);
    };

    class Playing : public GameFSM::State {
    public:
        void update(GameFSM::FullControl& control);
        void react(const RenderEvent& event, GameFSM::EventControl& control);
    };

    class Paused : public GameFSM::State {
    public:
        void enter(GameFSM::Control& control);
        void update(GameFSM::FullControl& control);
        void react(const RenderEvent& event, GameFSM::EventControl& control);
    };

    class StageComplete : public GameFSM::State {
    public:
        void enter(GameFSM::Control& control);
        void update(GameFSM::FullControl& control);
        void react(const RenderEvent& event, GameFSM::EventControl& control);
    };

    class GameOver : public GameFSM::State {
    public:
        void enter(GameFSM::Control& control);
        void update(GameFSM::FullControl& control);
        void react(const RenderEvent& event, GameFSM::EventControl& control);
    };
}