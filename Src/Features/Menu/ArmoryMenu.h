#pragma once

#include <functional>
#include <string>
#include <array>
#include <vector>
#include <raylib.h>
#include "Engine/Core.h"
#include "Engine/Graphics.h"
#include "Engine/Navigation.h"
#include "../Character/Character.h"
#include "../Character/NPC/Ally.h"
#include "../Character/Weapon/Gun/GunAttachment.h"

class ArmoryMenu : public Screen {
public:
	ArmoryMenu(
		Character& character,
		std::vector<UniquePtr<Ally>>& allies, 
		int& coins,
		std::array<size_t, AttachmentCount>& stock, 
		std::string backLabel = "BACK"
	);

	void update(float deltaTime, NavigationStack& navigation) override;
	void render(Engine::Graphics::Renderer& renderer) override;

private:
	enum class Tab { Weapons, Allies };

	struct Control {
		Rectangle bounds;
		std::string label;
		bool enabled;
		bool highlighted;
		std::function<void()> action;
	};

	struct Text {
		std::string value;
		Vector2 position;
		int size;
		Color color;
	};

	Character& player;
	std::vector<UniquePtr<Ally>>& allies;
	int& coin;

	Tab tab = Tab::Weapons;
	size_t selectedWeapon = 0;
	int hovered = -1;
	bool closeRequested = false;

	std::string message;
	float messageTimer = 0.0f;

	// Rebuilt every frame from the current game data.
	std::vector<Control> controls;
	std::vector<Text> texts;
	std::vector<Rectangle> panels;
	std::array<size_t, AttachmentCount>& stock;
	std::string backLabel;

	void build();
	void buildWeapons(float width, float height);
	void buildAllies(float width, float height);

	void addText(const std::string& value, Vector2 position, int size, Color color);
	void addControl(Rectangle bounds, const std::string& label, bool enabled, std::function<void()> action,
		bool highlighted = false);

	bool spend(int cost);
	void notify(const std::string& text);
};