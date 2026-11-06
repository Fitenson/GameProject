#pragma once

#include "State.h"

namespace GameState {
	class Loading : public GameFSM::State {
	public:
		void update(GameFSM::FullControl& control);
	};
}