// Stage.cpp
#include <algorithm>
#include "Stage.h"

Stage::Stage(const StageConfig& config) : config(config) {}

const StageConfig& Stage::getConfig() const { return config; }
int Stage::getNumber() const { return config.number; }
int Stage::getWave() const { return wave; }
int Stage::getWaveCount() const { return config.waveCount; }
int Stage::getKills() const { return kills; }
int Stage::getKillTarget() const { return config.killTarget; }
bool Stage::isResting() const { return resting; }
float Stage::getRestSeconds() const { return std::max(0.0f, restLeft); }

void Stage::registerKills(int count) { kills += count; }

bool Stage::isComplete() const
{
	return kills >= config.killTarget && (!config.requireAllGates || gatesOpen);
}

bool Stage::isWaitingForGates() const
{
	return config.requireAllGates && kills >= config.killTarget && !gatesOpen;
}

void Stage::startNextWave()
{
	++wave;
	waveRemaining = config.waveSizes[static_cast<size_t>(wave - 1)];
	resting = false;
	spawnTimer = 0.5f;
}

int Stage::update(float deltaTime, int aliveEnemies, bool allGatesOpen)
{
	gatesOpen = allGatesOpen;
	if (isComplete()) return 0;

	if (resting) {
		restLeft -= deltaTime;
		if (restLeft <= 0.0f && wave < config.waveCount) startNextWave();
		return 0;
	}

	// One spawn at a time, paced by spawnDelay, and never above the alive limit.
	spawnTimer = std::max(0.0f, spawnTimer - deltaTime);
	int spawn = 0;
	if (waveRemaining > 0 && spawnTimer <= 0.0f && aliveEnemies < config.maxAlive) {
		spawn = 1;
		--waveRemaining;
		spawnTimer = config.spawnDelay;
	}

	// Wave cleared: everything spawned and everything dead.
	if (spawn == 0 && waveRemaining == 0 && aliveEnemies == 0 && wave < config.waveCount) {
		resting = true;
		restLeft = config.waveRest;
	}
	return spawn;
}