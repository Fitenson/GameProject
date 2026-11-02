#pragma once

#include <vector>
#include <variant>
#include "Engine/Core.h"
#include "Engine/Graphics.h"
#include "../Character/Weapon/Gun/Attachment/GunPart.h"
#include "../Character/Weapon/Sword/Attachment/SwordPart.h"
#include "../Character/Item.h"

class Map;

class ItemSpawner {
public:
	ItemSpawner() = default;
	~ItemSpawner() = default;

	Item getItem();
	void setItem(Item item);

	Vector2 pickSpawnPoint(const Vector2& center, float viewRadius, const Map& map) const;
	void spawn(const Vector2& center, float viewRadius, const Map& map);
	std::vector<Item> collect(const Vector2& playerPosition, float reach);
	void clear();
	void render(Engine::Graphics::Renderer& renderer, const Vector2& viewCenter, float viewRadius) const;
	int count() const;


private:
	std::vector<Item> items;
};