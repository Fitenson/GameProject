// Stage.h
#pragma once

#include "StageConfig.h"

class Stage {
public:
	explicit Stage(const StageConfig& config);

	// Returns how many enemies should spawn this frame.
	int update(float deltaTime, int aliveEnemies, bool allGatesOpen);
	void registerKills(int count);

	bool isComplete() const;
	bool isWaitingForGates() const;   // kills reached, gates still closed
	bool isResting() const;
	float getRestSeconds() const;

	int getNumber() const;
	int getWave() const;
	int getWaveCount() const;
	int getKills() const;
	int getKillTarget() const;
	const StageConfig& getConfig() const;

private:
	static constexpr float StartDelay = 3.0f;

	StageConfig config;
	int kills = 0;
	int wave = 0;                 // 1-based number of the current wave, 0 = not started
	int waveRemaining = 0;        // enemies of this wave still to spawn
	bool resting = true;
	float restLeft = StartDelay;
	float spawnTimer = 0.0f;
	bool gatesOpen = false;

	void startNextWave();
};