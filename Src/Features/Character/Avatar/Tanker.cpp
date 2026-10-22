#include "Tanker.h"

Tanker::Tanker() {
	this->role = "Tanker";
	this->stats = {
		.hp = 250,
		.maxHp = 250,
		.damage = 25,
		.armor = 40,
		.movementSpeed = 20,
		.attackSpeed = 15
	};
}

void Tanker::specialAbility() const {}

float Tanker::getShieldRatio() const { return 0.5f; }