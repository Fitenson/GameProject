#pragma once

#include "Engine/Core.h"
#include "GameStates.h"
#include "GameContext.h"

class Game
{
public:
	explicit Game(Engine::Application& app);

	void update(float deltaTime);
	void render();

	template<typename State>
	bool isState() { return state.isActive<State>(); }

	template<typename State>
	void changeState() { state.changeTo<State>(); }

private:
	GameContext context;
	GameFSM::Instance state;
};