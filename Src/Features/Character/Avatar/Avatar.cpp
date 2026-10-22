#include "Avatar.h"

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