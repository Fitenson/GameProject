#pragma once

#include <array>
#include <functional>
#include <raylib.h>
#include "Engine/Graphics.h"
#include "Engine/UI.h"
#include "Engine/Navigation.h"
#include "../World/Map/StageConfig.h"

class StageScreen : public Screen {
public:
	StageScreen(int unlockedStages, std::function<void(int)> onSelect);

	void update(float deltaTime, NavigationStack& navigation) override;
	void render(Engine::Graphics::Renderer& renderer) override;

private:
	int unlocked;
	std::function<void(int)> onSelect;
	std::array<StageConfig, StageConfig::Count> stages;
	int hovered = -1;
	int screenWidth = 0;
	int screenHeight = 0;
	Engine::UI::Button backButton;

	Rectangle cardBounds(int index) const;
	void layout();
};