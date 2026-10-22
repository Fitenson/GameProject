#pragma once

#include <string>
#include "Engine/Navigation.h"
#include "Engine/UI.h"
#include "../Character/Avatar/Avatar.h"

class MainMenu: public Screen
{
public:
	MainMenu(std::function<void(AvatarVariant)> onStart, std::function<void()> onExit);

	void update(float deltaTime, NavigationStack& navigation) override;
	void render(Engine::Graphics::Renderer& renderer) override;

private:
	Engine::UI::Label title;
	Engine::UI::Button playButton;
	Engine::UI::Button exitButton;
	int screenWidth = 0;
	int screenHeight = 0;

	std::function<void(AvatarVariant)> onStart;
	std::function<void()> onQuit;

	void refresh();
};