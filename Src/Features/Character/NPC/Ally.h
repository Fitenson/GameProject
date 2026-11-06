#pragma once

#include <vector>
#include <raylib.h>
#include "Engine/Graphics.h"
#include "../../Game/Entity.h"
#include "../../Character/Character.h"
#include "../../Monster/Enemy.h"

class Ally : public Entity {
public:
	Ally(Character character, const Vector2& position);

	Character& getCharacter();

	void setLeader(const Vector2& leaderPosition);

	// preferred: distance to keep from the target. engage: how far away it notices enemies.
	// attack: it only pulls the trigger when the target is this close.
	void setCombatStyle(float preferred, float engage, float attack);

	// A summon that disappears after `seconds`.
	void makeTemporary(float seconds);
	bool isTemporary() const;
	bool isExpired() const;

	void update(float deltaTime) override;                                   // movement
	void updateCombat(float deltaTime, const std::vector<Enemy*>& targets);  // targeting and attacks
	void render(Engine::Graphics::Renderer& renderer) const override;

	Vector2 getHitPosition() const;
	float getHitRadius() const;
	void takeDamage(float amount);
	bool isDead() const;
	void restoreHealth(float fraction);

private:
	static constexpr float FollowDistance = 40.0f;
	static constexpr float TeleportDistance = 400.0f;
	static constexpr float LeashRange = 320.0f;   // ignores enemies farther than this from the leader
	static constexpr float BodyRadius = 10.0f;
	static constexpr float InvulnerableTime = 0.3f;
	static constexpr float ExpireWarning = 2.0f;  // a clone blinks for its last seconds

	Character character;
	Vector2 leader{ 0.0f, 0.0f };
	Vector2 aimDirection{ 1.0f, 0.0f };

	float preferredDistance = 150.0f;
	float engageRange = 240.0f;
	float attackRange = 240.0f;

	bool hasTarget = false;
	Vector2 targetPosition{ 0.0f, 0.0f };

	bool temporary = false;
	float lifetimeLeft = 0.0f;
	float invulnerableTimer = 0.0f;
	float strafeClock = 0.0f;
	float walkClock = 0.0f;
	float stride = 0.0f;
};