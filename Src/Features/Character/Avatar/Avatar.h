#pragma once

#include <string>
#include "Engine/Graphics.h"
#include "../Stats.h"
#include "AvatarLook.h"
#include "AvatarPose.h"

enum class AvatarVariant { Soldier, Assasin, Tanker };

class Avatar {
public:
	Avatar() = default;
	virtual ~Avatar() = default;

	float& getHp();
	const float& getHp() const;

	const float& getMaxHp() const;

	virtual float& getDamage();
	virtual const float& getDamage() const;
	virtual void setDamage(float& damage);

	virtual float& getArmor();
	virtual const float& getArmor() const;
	virtual void setArmor(float& armor);

	virtual float& getMovementSpeed();
	virtual const float& getMovementSpeed() const;
	virtual void setMovementSpeed(float& movementSpeed);

	virtual float& getAttackSpeed();
	virtual const float& getAttackSpeed() const;
	virtual void setAttackSpeed(float& attackSpeed);

	const std::string& getRole() const;

	// ---------- Special ability (Q key) ----------
	// Returns true if the ability started.
	bool activateAbility();
	void updateAbility(float deltaTime);

	bool isAbilityActive() const;
	float getAbilityTimer() const;
	float getAbilityCooldownLeft() const;

	virtual std::string getAbilityName() const = 0;
	virtual float getAbilityDuration() const = 0;
	virtual float getAbilityCooldown() const = 0;

	// What the ability changes. The defaults mean "no effect".
	virtual float getFireRateMultiplier() const { return 1.0f; }
	virtual float getSpeedMultiplier() const { return 1.0f; }
	virtual bool hasUnlimitedAmmo() const { return false; }
	virtual float getShieldBonusRatio() const { return 0.0f; }   // fraction of max HP
	virtual bool summonsClone() const { return false; }

	virtual float getShieldRatio() const { return 0.0f; }

	virtual AvatarLook getLook() const { return AvatarLook{}; }
	void renderBody(Engine::Graphics::Renderer& renderer, const AvatarPose& pose) const;
	void renderUpper(Engine::Graphics::Renderer& renderer, const AvatarPose& pose) const;

protected:
	virtual void specialAbility() {}   // runs once when the ability starts
	virtual void endAbility() {}       // runs once when it ends

	std::string role;
	Stats stats;
	float abilityTimer = 0.0f;
	float abilityCooldownLeft = 0.0f;
};