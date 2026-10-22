#pragma once

#include <cstdint>
#include <vector>
#include "../Puzzle/Puzzle.h"

struct EnemyScale {
	int level = 1;
	float health = 1.0f;
	float speed = 1.0f;
	float damage = 1.0f;
};

struct StageConfig {
	static constexpr int Count = 10;
	static constexpr int BaseMapCells = 128;
	static constexpr int MapGrowth = 8;

	int number = 1;
	uint64_t seed = 0;

	// Map
	int mapCells = BaseMapCells;
	int complexity = 1;
	float mainRoadHalfWidth = 6.0f;
	float trailHalfWidth = 4.5f;
	int wobbleWaves = 1;
	int decoyTrails = 1;
	int loops = 0;

	// Puzzles
	std::vector<PuzzleKind> puzzles;          // one per gate, west to east
	int leversPerGate = 1;
	int keysPerGate = 1;
	int timedSwitches = 2;
	float timedWindow = 16.0f;
	int runeCount = 3;
	bool requireAllGates = false;

	// Enemies
	EnemyScale enemy;

	// Waves
	int waveCount = 2;
	std::vector<int> waveSizes;
	int killTarget = 0;
	int maxAlive = 8;                         // enemies alive at once
	float spawnDelay = 1.5f;
	float waveRest = 8.0f;

	static StageConfig forStage(int number);
};