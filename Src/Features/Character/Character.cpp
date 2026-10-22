#include <utility>
#include "Character.h"

Character::Character(const std::string& name, int level, UniquePtr<Avatar> avatar):
name(name), level(level), avatar(std::move(avatar)) {}

std::string& Character::getName() { return name; }
void Character::setName(const std::string& name) { this->name = name; }

int& Character::getLevel() { return level; }
const int& Character::getLevel() const { return level; }
void Character::setLevel(int& level) { this->level = level; }

Avatar& Character::getAvatar() { return *avatar; }

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

void Character::equip(UniquePtr<Weapon> weapon)
{
	if (weapon) weapons.push_back(std::move(weapon));
}

void Character::selectWeapon(int index)
{
	if (index < weapons.size()) current = index;
}

void Character::selectNextWeapon()
{
	if (!weapons.empty()) current = (current + 1) % weapons.size();
}

void Character::selectPreviousWeapon()
{
	if (!weapons.empty()) current = (current + weapons.size() - 1) % weapons.size();
}

const std::string& Character::getCurrentWeaponName() const
{
	static const std::string none = "None";
	return weapons.empty() ? none : weapons[current]->getName();
}

void Character::updateWeapons(float deltaTime, const AttackContext& context, bool triggerPressed, bool triggerHeld)
{
	for (size_t i = 0; i < weapons.size(); ++i) {
		const bool equipped = (i == current);
		weapons[i]->update(deltaTime, context, equipped && triggerPressed, equipped && triggerHeld);
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
		weapons[current]->renderHeld(renderer, playerPosition, direction);
	}
}

void Character::update(float deltaTime)
{
	if (shield) shield->update(deltaTime);
}

void Character::equipShield(UniquePtr<Shield> newShield) { shield = std::move(newShield); }
const Shield* Character::getShield() const { return shield.get(); }

float Character::getMovementMultiplier() const
{
	return weapons.empty() ? 1.0f : weapons[current]->getMovementMultiplier();
}