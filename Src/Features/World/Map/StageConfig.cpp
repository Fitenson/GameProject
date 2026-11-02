#include <algorithm>
#include "StageConfig.h"
#include "Engine/Core.h"

StageConfig StageConfig::forStage(int number)
{
	StageConfig config;
	config.number = std::clamp(number, 1, Count);
	const int n = config.number;
	const float t = static_cast<float>(n - 1);

	// The seeder: one fixed seed per stage. Change the constant for a different set of worlds.
	config.seed = 0x5EED2024ull + static_cast<uint64_t>(n) * 0x9E3779B97F4A7C15ull;

	// Map
	config.mapCells = BaseMapCells + MapGrowth * (n - 1);
	config.complexity = n;
	config.trailHalfWidth = 4.5f - 0.1f * t;
	config.wobbleWaves = 1 + n / 4;
	config.decoyTrails = 1 + n / 2;
	config.loops = n / 2;

	// Puzzles: a new kind unlocks at certain stages and always opens that stage.
	struct Unlock { PuzzleKind kind; int stage; };
	static const Unlock unlocks[] = {
		{ PuzzleKind::Lever, 1 }, { PuzzleKind::Key, 2 }, { PuzzleKind::TimedSwitch, 4 },
		{ PuzzleKind::RuneSequence, 6 }, { PuzzleKind::KillGate, 8 }
	};

	std::vector<PuzzleKind> pool;
	PuzzleKind newest = PuzzleKind::Lever;
	bool hasNew = false;
	for (const Unlock& u : unlocks) {
		if (u.stage > n) continue;
		pool.push_back(u.kind);
		if (u.stage == n) { newest = u.kind; hasNew = true; }
	}

	SeededRandom rng(config.seed);
	const int gateCount = std::min(4, 1 + n / 3);
	for (int i = 0; i < gateCount; ++i) {
		if (i == 0 && hasNew) config.puzzles.push_back(newest);
		else config.puzzles.push_back(pool[static_cast<size_t>(rng.integer(0, static_cast<int>(pool.size()) - 1))]);
	}

	config.leversPerGate = 1 + n / 4;
	config.keysPerGate = n >= 6 ? 2 : 1;
	config.timedSwitches = n >= 7 ? 3 : 2;
	config.timedWindow = std::clamp(16.0f - 0.6f * t, 9.0f, 16.0f);
	config.runeCount = n >= 9 ? 4 : 3;
	config.requireAllGates = n >= 6;

	// Enemies: stronger and faster every stage.
	config.enemy.level = n;
	config.enemy.health = 1.0f + 0.30f * t;
	config.enemy.speed = 1.0f + 0.08f * t;
	config.enemy.damage = 1.0f + 0.22f * t;

	// Waves
	config.waveCount = 2 + n / 2;
	for (int w = 0; w < config.waveCount; ++w) {
		const int size = 4 + n + 2 * w;
		config.waveSizes.push_back(size);
		config.killTarget += size;
	}
	config.maxAlive = 6 + 2 * n;
	config.spawnDelay = std::max(0.35f, 1.6f - 0.12f * t);
	config.waveRest = std::max(3.0f, 8.0f - 0.5f * t);

	return config;
}