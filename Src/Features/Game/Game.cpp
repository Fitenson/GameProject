#include "Game.h"

Game::Game(Engine::Application& app) : context(app), state(context) {}

void Game::update(float deltaTime) { 
	context.deltaTime = deltaTime;
	state.update();
}

void Game::render() {
	state.react(RenderEvent{});
}