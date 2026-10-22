#pragma once

#include <string>
#include "Engine/Graphics.h"
#include "../Combat/AttackContext.h"

class Weapon {
public:
	virtual ~Weapon() = default;

	bool isFiring() const;
	float getMovementMultiplier() const;

	void update(float deltaTime, const AttackContext& context, bool triggerPressed, bool triggerHeld);
	virtual void renderHeld(Engine::Graphics::Renderer& renderer, const Vector2& origin, const Vector2& direction) const = 0;
	virtual void renderEffects(Engine::Graphics::Renderer& renderer) const {}

	const std::string& getName() const;

protected:
	Weapon(const std::string& name, float cooldown, float damageMultiplier, bool automatic);

	virtual void onAttack(const AttackContext& context) = 0;
	virtual void onUpdate(float deltaTime, const AttackContext& context) {}
	void setMovementMultiplier(float multiplier);

	float damageOf(const AttackContext& context) const;
	static bool inCone(const Vector2& origin, const Vector2& direction, const Vector2& targetPosition, float targetRadius, float range, float halfAngleRadians);

private:
	static constexpr float FiringGrace = 0.2f;   // stays "firing" briefly after the trigger is released

	std::string name;
	float cooldown;
	float cooldownLeft = 0.0f;
	float damageMultiplier;
	float movementMultiplier = 1.0f;
	float firingTimer = 0.0f;
	bool automatic;
};