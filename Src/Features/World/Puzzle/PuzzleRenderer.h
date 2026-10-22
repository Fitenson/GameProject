#pragma once

#include <vector>
#include <raylib.h>
#include "Engine/Graphics.h"
#include "Puzzle.h"

class PuzzleRenderer {
public:
	static void draw(Engine::Graphics::Renderer& renderer,
		const std::vector<PuzzleItem>& items, const std::vector<PuzzleGate>& gates,
		float cellSize, const Vector2& viewCenter, float viewRadius);
};