#pragma once

enum class TileType { Grass, Path, Tree, Rock, Wall, Gate, SwitchOff, SwitchOn, Cage };

inline bool isBlocking(TileType type) {
	return type == TileType::Tree ||
	type == TileType::Rock ||
	type == TileType::Wall ||
	type == TileType::Gate ||
	type == TileType::Cage;
}