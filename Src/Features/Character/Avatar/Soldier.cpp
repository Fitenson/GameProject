#include "Soldier.h"

Soldier::Soldier() {
	this->role = "Soldier";
	this->stats = {
		.hp = 100,
		.maxHp = 100,
		.damage = 15,
		.armor = 20,
		.movementSpeed = 30,
		.attackSpeed = 20
	};
}

void Soldier::specialAbility() const {}