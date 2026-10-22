#include <algorithm>
#include <cmath>
#include "EnemySpawner.h"
#include "../World/Map/Map.h"
#include "../World/Map/Chunk.h"
#include "../World/Map/Cell.h"

void EnemySpawner::configure(const EnemyScale& newScale) { scale = newScale; }

std::vector<Zombie>& EnemySpawner::getZombies() { return zombies; }
const std::vector<Zombie>& EnemySpawner::getZombies() const { return zombies; }

void EnemySpawner::clear() { zombies.clear(); kills = 0; }

int EnemySpawner::getAlive() const
{
	return static_cast<int>(std::count_if(zombies.begin(), zombies.end(), [](const Zombie& z) { return !z.isDead(); }));
}

int EnemySpawner::consumeKills()
{
	const int result = kills;
	kills = 0;
	return result;
}

float EnemySpawner::spawnDistance(float viewRadius) const
{
	const float chunkSize = static_cast<float>(Chunk::Width * Cell::Size);
	return viewRadius + spawnChunksBeyondView * chunkSize;
}

// A point on the ring just outside the vision circle, inside the map and off obstacles when possible.
Vector2 EnemySpawner::pickSpawnPoint(const Vector2& center, float viewRadius, const Map& map) const
{
	const Rectangle bounds = map.getPixelBounds();
	const float distance = spawnDistance(viewRadius);
	const float margin = 16.0f;

	Vector2 point = center;
	for (int attempt = 0; attempt < 16; ++attempt) {
		const float angle = GetRandomValue(0, 35999) / 100.0f * DEG2RAD;
		point = Vector2{ center.x + std::cos(angle) * distance, center.y + std::sin(angle) * distance };

		const bool inMap = point.x > bounds.x + margin && point.x < bounds.x + bounds.width - margin &&
			point.y > bounds.y + margin && point.y < bounds.y + bounds.height - margin;
		if (inMap && !map.isBlocked(point)) return point;
	}

	// Fallback: never fail, or the stage would wait for an enemy that does not exist.
	point.x = std::clamp(point.x, bounds.x + margin, bounds.x + bounds.width - margin);
	point.y = std::clamp(point.y, bounds.y + margin, bounds.y + bounds.height - margin);
	return point;
}

void EnemySpawner::spawn(const Vector2& position)
{
	zombies.emplace_back(position);
	zombies.back().applyLevel(scale.level, scale.health, scale.speed, scale.damage);
}

void EnemySpawner::update(float deltaTime, std::vector<Target>& targets, const Vector2& center, float viewRadius,
	int spawnCount, const Map& map)
{
	for (int i = 0; i < spawnCount; ++i) spawn(pickSpawnPoint(center, viewRadius, map));

	const float limit = spawnDistance(viewRadius) * 1.6f;
	const float limitSquared = limit * limit;

	for (Zombie& zombie : zombies) {
		// Each zombie chases and attacks the nearest living target.
		Target* nearest = nullptr;
		float best = 1e18f;
		const Vector2 p = zombie.getPosition();

		for (Target& target : targets) {
			if (!target.alive) continue;
			const float dx = target.position.x - p.x;
			const float dy = target.position.y - p.y;
			const float d = dx * dx + dy * dy;
			if (d < best) { best = d; nearest = &target; }
		}

		if (nearest) zombie.setTarget(nearest->position);
		zombie.update(deltaTime);
		if (nearest) nearest->damage += zombie.updateAttack(deltaTime, nearest->position, nearest->radius);

		if (zombie.claimKill()) { ++kills; continue; }
		if (zombie.isDead()) continue;

		// Fell far behind the player: move it back to the ring instead of losing it.
		const float dx = p.x - center.x, dy = p.y - center.y;
		if (dx * dx + dy * dy > limitSquared) zombie.setPosition(pickSpawnPoint(center, viewRadius, map));
	}

	std::erase_if(zombies, [](const Zombie& zombie) { return zombie.isRemovable(); });
}

// Zombies are only drawn inside the vision circle.
void EnemySpawner::render(Engine::Graphics::Renderer& renderer, const Vector2& viewCenter, float viewRadius) const
{
	const float radiusSquared = viewRadius * viewRadius;

	for (const Zombie& zombie : zombies) {
		const Vector2 p = zombie.getPosition();
		const float dx = p.x - viewCenter.x;
		const float dy = p.y - viewCenter.y;
		if (dx * dx + dy * dy <= radiusSquared) zombie.render(renderer);
	}
}

void EnemySpawner::collectTargets(std::vector<Enemy*>& out, const Vector2& viewCenter, float viewRadius)
{
	const float radiusSquared = viewRadius * viewRadius;

	for (Zombie& zombie : zombies) {
		if (zombie.isDead()) continue;

		const Vector2 p = zombie.getPosition();
		const float dx = p.x - viewCenter.x;
		const float dy = p.y - viewCenter.y;
		if (dx * dx + dy * dy <= radiusSquared) out.push_back(&zombie);
	}
}