#pragma once

#include <raylib.h>

class Enemy {
public:
	virtual ~Enemy() = default;

	virtual Vector2 getHitPosition() const = 0;
	virtual float getHitRadius() const = 0;

	virtual void takeDamage(float damage) = 0;
	virtual void stun(float seconds) = 0;
	virtual void renderDeath(Engine::Graphics::Renderer& renderer) const = 0;
	virtual bool isDead() const = 0;
	virtual bool isRemovable() const = 0;
	virtual float updateAttack(float deltaTime, const Vector2& targetPosition, float targetRadius) = 0;

	bool claimKill()
	{
		if (killClaimed || !isDead()) return false;
		killClaimed = true;
		return true;
	}

	static constexpr float HealthBarDisplayDuration = 5.0f;
	static constexpr float HealthBarFade = 1.0f;
	static constexpr float TrailDelay = 0.5f;
	static constexpr float HitFlashDuration = 0.1f;
	static constexpr float DeathDuration = 0.6f;
	static constexpr float AttackReach = 8.0f;      // extra distance beyond touching
	static constexpr float AttackWindup = 0.3f;     // telegraph before the hit lands
	static constexpr float AttackCooldown = 1.0f;
	static constexpr float StopDistance = 26.0f;    // stops walking when this close

protected:
	float healthBarTimer = 0.0f;
	float trailHp = 100.0f;
	float trailDelay = 0.0f;
	float hitFlash = 0.0f;
	float stunTimer = 0.0f;
	float deathTimer = 0.0f;
	float attackCooldownLeft = 0.0f;
	float windupLeft = 0.0f;
	bool attacking = false;
	bool killClaimed = false;
};