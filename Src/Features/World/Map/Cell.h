#pragma once

#include<cstdint>
#include <raylib.h>
#include "./TileType.h"

class Cell {
public:
	static const uint32_t Size = 8;

	Cell(const Vector2& position, const TileType& type);

	const Vector2& getPosition() const;
	const TileType& getType() const;

private:
	Vector2 position;
	TileType type;
};


inline uint32_t hashCell(int x, int y)
{
	uint32_t hash = static_cast<uint32_t>(x) * 374761393u + static_cast<uint32_t>(y) * 668265263u;
	hash = (hash ^ (hash >> 13)) * 1274126177u;
	return hash ^ (hash >> 16);
}