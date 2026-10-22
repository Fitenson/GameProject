#pragma once

#include <vector>
#include <raylib.h>
#include "TileType.h"
#include "../Puzzle/Puzzle.h"
#include "../Map/StageConfig.h"

struct MapLayout {
	int cellsX = 0;
	int cellsY = 0;
	std::vector<TileType> tiles;          // cellsX * cellsY, row by row
	std::vector<Vector2> waypoints;       // main road centreline, in pixels
	Vector2 spawn{ 0.0f, 0.0f };          // in pixels
	std::vector<PuzzleItem> items;
	std::vector<PuzzleGate> gates;
	int cageX = 0;                        // centre cell of the 3x3 prison cage
	int cageY = 0;
};

class MapGenerator {
public:
	static MapLayout generate(const StageConfig& config);
};