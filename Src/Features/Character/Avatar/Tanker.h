#pragma once

#include "Avatar.h"

class Tanker : public Avatar {
public:
	Tanker();

	AvatarLook getLook() const override;

	std::string getAbilityName() const override;
	float getAbilityDuration() const override { return 8.0f; }
	float getAbilityCooldown() const override { return 25.0f; }

	// Fortify: every stat +30%.
	float getFireRateMultiplier() const override { return isAbilityActive() ? Boost : 1.0f; }
	float getSpeedMultiplier() const override { return isAbilityActive() ? Boost : 1.0f; }
	float getShieldBonusRatio() const override { return 0.15f; }   // +30% of the base shield (0.5 x max HP)

	float getShieldRatio() const override;

protected:
	void specialAbility() override;
	void endAbility() override;

private:
	static constexpr float Boost = 1.3f;
};