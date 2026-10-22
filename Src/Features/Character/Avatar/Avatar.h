#pragma once

#include <string>
#include "../Stats.h"

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

	virtual void specialAbility() const = 0;

	virtual float getShieldRatio() const { return 0.0f; }

protected:
	std::string role;
	Stats stats;
};