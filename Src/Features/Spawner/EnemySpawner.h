#pragma once

#include <vector>
#include <raylib.h>
#include "Engine/Graphics.h"
#include "../World/Map/StageConfig.h"
#include "../Character/Combat/Target.h"
#include "../Monster/Zombie.h"

class Map;

class EnemySpawner {
public:
	EnemySpawner() = default;

	void configure(const EnemyScale& scale);   // level applied to every new zombie

	// spawnCount: how many zombies the stage asks for this frame.
	void update(float deltaTime, std::vector<Target>& targets, const Vector2& center, float viewRadius,
		int spawnCount, const Map& map);
	void render(Engine::Graphics::Renderer& renderer, const Vector2& viewCenter, float viewRadius) const;
	void collectTargets(std::vector<Enemy*>& out, const Vector2& viewCenter, float viewRadius);

	int getAlive() const;        // not counting dying zombies
	int consumeKills();          // kills since the last call
	void clear();

	std::vector<Zombie>& getZombies();
	const std::vector<Zombie>& getZombies() const;

private:
	std::vector<Zombie> zombies;
	EnemyScale scale;
	int kills = 0;
	int spawnChunksBeyondView = 2;

	float spawnDistance(float viewRadius) const;
	Vector2 pickSpawnPoint(const Vector2& center, float viewRadius, const Map& map) const;
	void spawn(const Vector2& position);
};