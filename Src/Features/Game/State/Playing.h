#pragma once

#include "State.h"

namespace GameState {
    class Playing : public GameFSM::State {
    public:
        void update(GameFSM::FullControl& control);
        void react(const RenderEvent& event, GameFSM::EventControl& control);
    };
}