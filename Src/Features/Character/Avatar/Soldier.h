#pragma once

#include "Avatar.h"

class Soldier : public Avatar {
public:
	Soldier();

	std::string getAbilityName() const override;
	float getAbilityDuration() const override { return 6.0f; }
	float getAbilityCooldown() const override { return 20.0f; }

	// Overdrive: double fire rate, infinite ammo, no reload.
	float getFireRateMultiplier() const override { return isAbilityActive() ? 2.0f : 1.0f; }
	bool hasUnlimitedAmmo() const override { return isAbilityActive(); }


private:
    static constexpr float OverdriveMultiplier = 1.25f;
    static constexpr float AbilityDuration = 6.0f;

    float originalDamage = 0.0f;
    float originalArmor = 0.0f;
    float originalMovementSpeed = 0.0f;
    float originalAttackSpeed = 0.0f;
	bool activeAbility;
};