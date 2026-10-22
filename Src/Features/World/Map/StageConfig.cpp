#include <algorithm>
#include "StageConfig.h"
#include "Engine/Core.h"

StageConfig StageConfig::forStage(int number)
{
	StageConfig c;
	c.number = std::clamp(number, 1, Count);
	const int n = c.number;
	const float t = static_cast<float>(n - 1);

	// The seeder: one fixed seed per stage. Change the constant for a different set of worlds.
	c.seed = 0x5EED2024ull + static_cast<uint64_t>(n) * 0x9E3779B97F4A7C15ull;

	// Map
	c.mapCells = BaseMapCells + MapGrowth * (n - 1);
	c.complexity = n;
	c.trailHalfWidth = 4.5f - 0.1f * t;
	c.wobbleWaves = 1 + n / 4;
	c.decoyTrails = 1 + n / 2;
	c.loops = n / 2;

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

	SeededRandom rng(c.seed);
	const int gateCount = std::min(4, 1 + n / 3);
	for (int i = 0; i < gateCount; ++i) {
		if (i == 0 && hasNew) c.puzzles.push_back(newest);
		else c.puzzles.push_back(pool[static_cast<size_t>(rng.integer(0, static_cast<int>(pool.size()) - 1))]);
	}

	c.leversPerGate = 1 + n / 4;
	c.keysPerGate = n >= 6 ? 2 : 1;
	c.timedSwitches = n >= 7 ? 3 : 2;
	c.timedWindow = std::clamp(16.0f - 0.6f * t, 9.0f, 16.0f);
	c.runeCount = n >= 9 ? 4 : 3;
	c.requireAllGates = n >= 6;

	// Enemies: stronger and faster every stage.
	c.enemy.level = n;
	c.enemy.health = 1.0f + 0.30f * t;
	c.enemy.speed = 1.0f + 0.08f * t;
	c.enemy.damage = 1.0f + 0.22f * t;

	// Waves
	c.waveCount = 2 + n / 2;
	for (int w = 0; w < c.waveCount; ++w) {
		const int size = 4 + n + 2 * w;
		c.waveSizes.push_back(size);
		c.killTarget += size;
	}
	c.maxAlive = 6 + 2 * n;
	c.spawnDelay = std::max(0.35f, 1.6f - 0.12f * t);
	c.waveRest = std::max(3.0f, 8.0f - 0.5f * t);

	return c;
}