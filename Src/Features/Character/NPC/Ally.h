// Ally.h
#pragma once

#include <vector>
#include <raylib.h>
#include "Engine/Graphics.h"
#include "../../Game/Entity.h"
#include "../Character.h"
#include "../../Monster/Enemy.h"

class Ally : public Entity {
public:
	Ally(Character character, const Vector2& position);

	void setLeader(const Vector2& leaderPosition);

	Vector2 getHitPosition() const;
	float getHitRadius() const;
	void takeDamage(float amount);
	bool isDead() const;

	void update(float deltaTime) override;   // follows the leader
	void updateCombat(float deltaTime, const std::vector<Enemy*>& targets);
	void render(Engine::Graphics::Renderer& renderer) const override;

private:
	static constexpr float FollowDistance = 40.0f;   // stops walking this close to the leader
	static constexpr float TeleportDistance = 400.0f;
	static constexpr float FireRange = 240.0f;
	static constexpr float BodyRadius = 10.0f;
	static constexpr float InvulnerableTime = 0.3f;

	Character character;
	Vector2 leader{ 0.0f, 0.0f };
	Vector2 aimDirection{ 1.0f, 0.0f };
	float invulnerableTimer = 0.0f;
};