#pragma once

#include <array>
#include <functional>
#include <string>
#include <raylib.h>
#include "Engine/Graphics.h"
#include "Engine/UI.h"
#include "Engine/Navigation.h"
#include "../Character/Avatar/Avatar.h"

struct AvatarOption {
	AvatarVariant variant;
	std::string name;
	float damage;
	float armor;
	float movementSpeed;
	Color color;
	Rectangle bounds{};
};

class AvatarMenu : public Screen {
public:
	explicit AvatarMenu(std::function<void(AvatarVariant)> onStart);

	void update(float deltaTime, NavigationStack& navigation) override;
	void render(Engine::Graphics::Renderer& renderer) override;

private:
	std::array<AvatarOption, 3> options;
	int selectedIndex = 0;
	int hoveredIndex = -1;

	Engine::UI::Button confirmButton;
	Engine::UI::Button backButton;
	Rectangle titleBounds{};
	int screenWidth = 0;
	int screenHeight = 0;

	std::function<void(AvatarVariant)> onStart;

	void refresh();
};