#pragma once

#include "Avatar.h"

class Tanker : public Avatar {
public:
	Tanker();

	AvatarLook getLook() const override;

	std::string getAbilityName() const override;
	float getAbilityDuration() const override { return 8.0f; }
	float getAbilityCooldown() const override { return 25.0f; }

	// Fortify: faster fire, faster movement, and a larger shield.
	float getFireRateMultiplier() const override { return isAbilityActive() ? 1.5f : 1.0f; }
	float getSpeedMultiplier() const override { return isAbilityActive() ? 1.4f : 1.0f; }
	float getShieldBonusRatio() const override { return 0.5f; }

	float getShieldRatio() const override;
};