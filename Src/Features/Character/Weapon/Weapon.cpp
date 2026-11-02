#include <algorithm>
#include <cmath>
#include "Weapon.h"

Weapon::Weapon(const std::string& name, float cooldown, float damageMultiplier, bool automatic)
	: name(name), cooldown(cooldown), damageMultiplier(damageMultiplier), automatic(automatic) {}

const std::string& Weapon::getName() const { return name; }

float Weapon::damageOf(const AttackContext& context) const
{
	return context.baseDamage * damageMultiplier * (1.0f + 0.2f * static_cast<float>(level - 1));
}

bool Weapon::isFiring() const { return firingTimer > 0.0f; }

float Weapon::getMovementMultiplier() const { return isFiring() ? movementMultiplier : 1.0f; }

AmmoInfo Weapon::getAmmoInfo() const { return {}; }

HandPose Weapon::getHandPose(const Vector2& origin, const Vector2& direction) const
{
	HandPose pose;
	pose.hasRight = true;
	pose.right = Vector2{ origin.x + direction.x * 14.0f - direction.y * 3.0f, origin.y + direction.y * 14.0f + direction.x * 3.0f };
	return pose;
}

int Weapon::getLevel() const { return level; }
void Weapon::setLevel(int level) { this->level = level; }
bool Weapon::isLevelMax() const { return level >= MaxLevel; }

std::string Weapon::getAttachmentName(const Attachment& attachment)
{
	return std::visit(
		[](const auto& value) -> std::string
		{
			return attachmentName(value);
		},
		attachment.attachment
	);
}

int Weapon::getUpgradeCost() const { return 40 * level; }

void Weapon::setMovementMultiplier(float multiplier) { movementMultiplier = multiplier; }
void Weapon::update(float deltaTime, const AttackContext& context, bool triggerPressed, bool triggerHeld)
{
	cooldownLeft = std::max(0.0f, cooldownLeft - deltaTime * context.rateMultiplier);
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

std::string Weapon::attachmentName(GunAttachment attachment)
{
	switch (attachment)
	{
	case GunAttachment::Scope:
		return "Scope";

	case GunAttachment::Compensator:
		return "Compensator";

	case GunAttachment::Suppressor:
		return "Suppressor";

	default:
		return "";
	}
}

std::string Weapon::attachmentName(SwordAttachment attachment)
{
	switch (attachment)
	{
	case SwordAttachment::Serrations:
		return "Serrations";

	case SwordAttachment::Fuller:
		return "Fuller";

	case SwordAttachment::Crescent:
		return "Crescent Tip";

	default:
		return "";
	}
}