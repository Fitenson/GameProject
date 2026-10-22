#pragma once

#include <cstdint>
#include <vector>
#include <raylib.h>
#include "Engine/Graphics.h"
#include "Cell.h"

class Chunk {
public:
	static const uint32_t Width = 8;
	static const uint32_t Height = 8;

	Chunk(const Vector2& position, const std::vector<TileType>& tiles);

	std::vector<Cell>& getCells();
	Vector2& getPosition();
	const Vector2& getPosition() const;

	Rectangle getBounds() const;
	void renderGround(Engine::Graphics::Renderer& renderer, const Vector2& center, float radius) const;
	void renderObjects(Engine::Graphics::Renderer& renderer, const Vector2& center, float radius) const;

private:
	std::vector<Cell> cells;
	Vector2 position;

	static bool insideCircle(const Vector2& p, const Vector2& center, float radius)
	{
		const float dx = p.x - center.x, dy = p.y - center.y;
		return dx * dx + dy * dy <= radius * radius;
	}
};