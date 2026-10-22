#pragma once

#include <string>
#include "Engine/Core.h"
#include "Engine/Graphics.h"
#include "Avatar/Avatar.h"
#include "../Character/Weapon/Weapon.h"
#include "../Character/Weapon/Shield/Shield.h"

class Character {
public:
	Character() = default;
	Character(const std::string& name, int level, UniquePtr<Avatar> avatar);

	std::string& getName();
	void setName(const std::string& name);

	int& getLevel();
	const int& getLevel() const;
	void setLevel(int& level);

	float& getDamage();
	const float& getDamage() const;
	void setDamage(float& damage);

	float& getArmor();
	const float& getArmor() const;
	void setArmor(float& armor);

	float& getMovementSpeed();
	const float& getMovementSpeed() const;
	void setMovementSpeed(float& speed);

	float& getAttackSpeed();
	const float& getAttackSpeed() const;
	void setAttackSpeed(float& speed);

	Avatar& getAvatar();
	const Avatar& getAvatar() const;

	float getHp() const;
	float getMaxHp() const;

	bool isDead() const;
	void takeDamage(float rawDamage);

	void equip(UniquePtr<Weapon> weapon);
	void equipShield(UniquePtr<Shield> shield);
	const Shield* getShield() const;   // nullptr when there is none

	void selectWeapon(int index);
	void selectNextWeapon();
	void selectPreviousWeapon();
	const std::string& getCurrentWeaponName() const;

	float getMovementMultiplier() const;

	void update(float deltaTime);   // ticks the shield
	void updateWeapons(float deltaTime, const AttackContext& context, bool triggerPressed, bool triggerHeld);

	void renderWeaponEffects(Engine::Graphics::Renderer& renderer) const;
	void renderHeldWeapon(Engine::Graphics::Renderer& renderer, const Vector2& origin, const Vector2& direction) const;


private:
	std::string name;
	int level;
	UniquePtr<Avatar> avatar;
	std::vector<UniquePtr<Weapon>> weapons;
	UniquePtr<Shield> shield;
	int current = 0;
};