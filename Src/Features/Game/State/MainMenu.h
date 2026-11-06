#pragma once

#include "State.h"

namespace GameState {
	class MainMenu : public GameFSM::State {
	public:
		void enter(GameFSM::Control& control);
		void update(GameFSM::FullControl& control);
		void react(const RenderEvent& event, GameFSM::EventControl& control);
	};
}