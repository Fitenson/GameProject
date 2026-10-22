#pragma once

#include <vector>
#include "Engine/Core.h"
#include "Engine/UI.h"
#include "Engine/Graphics.h"
#include "Engine/Navigation.h"
#include "../Character/Avatar/Avatar.h"
#include "../Character/Character.h"
#include "../Character/NPC/Ally.h"
#include "../Player/Player.h"
#include "../World/World.h"

namespace Engine { class Application; }

struct GameContext
{
	Engine::Application& app;
	Engine::Graphics::Renderer renderer;
	AvatarVariant selectedAvatar = AvatarVariant::Soldier;
	NavigationStack navigation;
	UniquePtr<Engine::UI::Dialog> dialog;
	bool startRequested = false;
	UniquePtr<Player> player;
	std::vector<UniquePtr<Ally>> allies;
	UniquePtr<World> world;
	float deltaTime = 0.0f;
	int selectedStage = 1;
	int unlockedStages = 1;
};