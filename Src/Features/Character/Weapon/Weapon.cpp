#include <algorithm>
#include <cmath>
#include "Weapon.h"

Weapon::Weapon(const std::string& name, float cooldown, float damageMultiplier, bool automatic)
	: name(name), cooldown(cooldown), damageMultiplier(damageMultiplier), automatic(automatic) {}

const std::string& Weapon::getName() const { return name; }

float Weapon::damageOf(const AttackContext& context) const
{
	return context.baseDamage * damageMultiplier;
}

bool Weapon::isFiring() const { return firingTimer > 0.0f; }
float Weapon::getMovementMultiplier() const { return isFiring() ? movementMultiplier : 1.0f; }
void Weapon::setMovementMultiplier(float multiplier) { movementMultiplier = multiplier; }

void Weapon::update(float deltaTime, const AttackContext& context, bool triggerPressed, bool triggerHeld)
{
	cooldownLeft = std::max(0.0f, cooldownLeft - deltaTime);
	firingTimer = std::max(0.0f, firingTimer - deltaTime);

	const bool wantsToAttack = automatic ? triggerHeld : triggerPressed;
	if (wantsToAttack) firingTimer = FiringGrace;   // the trigger is down, even while on cooldown

	if (wantsToAttack && cooldownLeft <= 0.0f) {
		cooldownLeft = cooldown;
		onAttack(context);
	}

	onUpdate(deltaTime, context);
}

bool Weapon::inCone(const Vector2& origin, const Vector2& direction,
	const Vector2& targetPosition, float targetRadius,
	float range, float halfAngleRadians)
{
	const float dx = targetPosition.x - origin.x;
	const float dy = targetPosition.y - origin.y;
	const float distance = std::sqrt(dx * dx + dy * dy);

	if (distance > range + targetRadius) return false;
	if (distance <= targetRadius) return true;   // overlapping the holder

	const float cosAngle = (dx * direction.x + dy * direction.y) / distance;
	return cosAngle >= std::cos(halfAngleRadians);
}