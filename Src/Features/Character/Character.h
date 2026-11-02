#pragma once

#include <cstddef>
#include <string>
#include <vector>
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
	void heal(float amount);   // restores HP, capped at the maximum

	void equip(UniquePtr<Weapon> weapon);
	void equip(UniquePtr<Shield> shield);
	const Shield* getShield() const;   // nullptr when there is none

	// Weapon access (used by the armory)
	size_t getWeaponCount() const;
	Weapon* getWeapon(size_t index);   // nullptr when out of range

	void selectWeapon(int index);
	void selectNextWeapon();
	void selectPreviousWeapon();
	const std::string& getCurrentWeaponName() const;
	AmmoInfo getCurrentAmmoInfo() const;   // ammo state of the weapon in hand

	float getMovementMultiplier() const;
	bool activateAbility();

	void update(float deltaTime);   // ticks the shield
	void updateWeapons(float deltaTime, const AttackContext& context, bool triggerPressed, bool triggerHeld);

	HandPose getHandPose(const Vector2& position, const Vector2& direction) const;

	void renderWeaponEffects(Engine::Graphics::Renderer& renderer) const;
	void renderHeldWeapon(Engine::Graphics::Renderer& renderer, const Vector2& origin, const Vector2& direction) const;

private:
	std::string name;
	int level = 1;
	UniquePtr<Avatar> avatar;
	std::vector<UniquePtr<Weapon>> weapons;
	UniquePtr<Shield> shield;
	int current = 0;
};