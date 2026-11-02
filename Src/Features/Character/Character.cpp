#include <algorithm>
#include <utility>
#include "Character.h"

Character::Character(const std::string& name, int level, UniquePtr<Avatar> avatar) :
	name(name), level(level), avatar(std::move(avatar)) {
}

std::string& Character::getName() { return name; }
void Character::setName(const std::string& name) { this->name = name; }

int& Character::getLevel() { return level; }
const int& Character::getLevel() const { return level; }
void Character::setLevel(int& level) { this->level = level; }

Avatar& Character::getAvatar() { return *avatar; }
const Avatar& Character::getAvatar() const { return *avatar; }

float& Character::getDamage() { return avatar->getDamage(); }
const float& Character::getDamage() const { return avatar->getDamage(); }
void Character::setDamage(float& damage) { avatar->setDamage(damage); }

float& Character::getArmor() { return avatar->getArmor(); }
const float& Character::getArmor() const { return avatar->getArmor(); }
void Character::setArmor(float& armor) { avatar->setArmor(armor); }

float& Character::getMovementSpeed() { return avatar->getMovementSpeed(); }
const float& Character::getMovementSpeed() const { return avatar->getMovementSpeed(); }
void Character::setMovementSpeed(float& movementSpeed) { avatar->setMovementSpeed(movementSpeed); }

float& Character::getAttackSpeed() { return avatar->getAttackSpeed(); }
const float& Character::getAttackSpeed() const { return avatar->getAttackSpeed(); }
void Character::setAttackSpeed(float& attackSpeed) { avatar->setAttackSpeed(attackSpeed); }

float Character::getHp() const { return avatar->getHp(); }
float Character::getMaxHp() const { return avatar->getMaxHp(); }
bool Character::isDead() const { return avatar->getHp() <= 0.0f; }

void Character::takeDamage(float rawDamage)
{
	if (isDead()) return;

	// The shield soaks the raw hit first. Only the leftover is reduced by armor.
	if (shield) rawDamage = shield->absorb(rawDamage);
	if (rawDamage <= 0.0f) return;

	const float mitigated = rawDamage * 100.0f / (100.0f + avatar->getArmor());
	avatar->getHp() = std::max(0.0f, avatar->getHp() - mitigated);
}

void Character::heal(float amount)
{
	if (isDead()) return;
	avatar->getHp() = std::min(avatar->getMaxHp(), avatar->getHp() + amount);
}

void Character::equip(UniquePtr<Weapon> weapon)
{
	if (weapon) weapons.push_back(std::move(weapon));
}

size_t Character::getWeaponCount() const { return weapons.size(); }

Weapon* Character::getWeapon(size_t index)
{
	return index < weapons.size() ? weapons[index].get() : nullptr;
}

void Character::selectWeapon(int index)
{
	if (index >= 0 && index < static_cast<int>(weapons.size())) current = index;
}

void Character::selectNextWeapon()
{
	if (!weapons.empty()) current = (current + 1) % static_cast<int>(weapons.size());
}

void Character::selectPreviousWeapon()
{
	if (!weapons.empty()) {
		const int count = static_cast<int>(weapons.size());
		current = (current + count - 1) % count;
	}
}

const std::string& Character::getCurrentWeaponName() const
{
	static const std::string none = "None";
	return weapons.empty() ? none : weapons[static_cast<size_t>(current)]->getName();
}

AmmoInfo Character::getCurrentAmmoInfo() const
{
	return weapons.empty() ? AmmoInfo{} : weapons[static_cast<size_t>(current)]->getAmmoInfo();
}

void Character::updateWeapons(float deltaTime, const AttackContext& context, bool triggerPressed, bool triggerHeld)
{
	for (size_t i = 0; i < weapons.size(); ++i) {
		const bool equipped = (static_cast<int>(i) == current);

		// Every weapon updates (bullets keep flying and reloads continue),
		// but only the weapon in hand gets the trigger and the reload key.
		AttackContext local = context;
		local.reloadRequested = context.reloadRequested && equipped;
		local.rateMultiplier = avatar->getFireRateMultiplier();
		local.unlimitedAmmo = avatar->hasUnlimitedAmmo();

		weapons[i]->update(deltaTime, local, equipped && triggerPressed, equipped && triggerHeld);
	}
}

void Character::renderWeaponEffects(Engine::Graphics::Renderer& renderer) const
{
	for (const UniquePtr<Weapon>& weapon : weapons) {
		weapon->renderEffects(renderer);
	}
}

void Character::renderHeldWeapon(Engine::Graphics::Renderer& renderer, const Vector2& playerPosition, const Vector2& direction) const
{
	if (!weapons.empty()) {
		weapons[static_cast<size_t>(current)]->renderHeld(renderer, playerPosition, direction);
	}
}

void Character::update(float deltaTime)
{
	if (shield) shield->update(deltaTime);
	avatar->updateAbility(deltaTime);
}

void Character::equip(UniquePtr<Shield> newShield) { shield = std::move(newShield); }

const Shield* Character::getShield() const { return shield.get(); }

HandPose Character::getHandPose(const Vector2& position, const Vector2& direction) const
{
	return weapons.empty() ? HandPose{} : weapons[static_cast<size_t>(current)]->getHandPose(position, direction);
}

float Character::getMovementMultiplier() const
{
	const float weapon = weapons.empty() ? 1.0f : weapons[static_cast<size_t>(current)]->getMovementMultiplier();
	return weapon * avatar->getSpeedMultiplier();
}

bool Character::activateAbility()
{
	return avatar->activateAbility();
}