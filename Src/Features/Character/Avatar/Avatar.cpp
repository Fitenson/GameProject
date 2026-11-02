#include "Avatar.h"
#include "AvatarPainter.h"

float& Avatar::getHp() { return stats.hp; }
const float& Avatar::getHp() const { return stats.hp; }

const float& Avatar::getMaxHp() const { return stats.maxHp; }

float& Avatar::getDamage() { return stats.damage; }
const float& Avatar::getDamage() const { return stats.damage; }
void Avatar::setDamage(float& damage) { this->stats.damage = damage; }

float& Avatar::getArmor() { return stats.armor; }
const float& Avatar::getArmor() const { return stats.armor; }
void Avatar::setArmor(float& armor) { this->stats.armor = armor; }

float& Avatar::getMovementSpeed() { return stats.movementSpeed; }
const float& Avatar::getMovementSpeed() const { return stats.movementSpeed; }
void Avatar::setMovementSpeed(float& movementSpeed) { this->stats.movementSpeed = movementSpeed; }

float& Avatar::getAttackSpeed() { return stats.attackSpeed; }
const float& Avatar::getAttackSpeed() const { return stats.attackSpeed; }
void Avatar::setAttackSpeed(float& attackSpeed) { this->stats.attackSpeed = attackSpeed; }

const std::string& Avatar::getRole() const { return role; }

bool Avatar::activateAbility()
{
	if (isAbilityActive() || abilityCooldownLeft > 0.0f) return false;

	abilityTimer = getAbilityDuration();
	specialAbility();
	return true;
}

void Avatar::updateAbility(float deltaTime)
{
	if (isAbilityActive()) {
		abilityTimer -= deltaTime;
		if (abilityTimer <= 0.0f) {
			abilityTimer = 0.0f;
			endAbility();
			abilityCooldownLeft = getAbilityCooldown();
		}
	}
	else if (abilityCooldownLeft > 0.0f) {
		abilityCooldownLeft = std::max(0.0f, abilityCooldownLeft - deltaTime);
	}
}

bool Avatar::isAbilityActive() const { return abilityTimer > 0.0f; }
float Avatar::getAbilityTimer() const { return abilityTimer; }
float Avatar::getAbilityCooldownLeft() const { return abilityCooldownLeft; }

void Avatar::renderBody(Engine::Graphics::Renderer& renderer, const AvatarPose& pose) const
{
	AvatarPainter::drawBody(renderer, pose, getLook());
}

void Avatar::renderUpper(Engine::Graphics::Renderer& renderer, const AvatarPose& pose) const
{
	AvatarPainter::drawUpper(renderer, pose, getLook());
}