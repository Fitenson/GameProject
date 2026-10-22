#include "Assasin.h"

Assasin::Assasin() {
	this->role = "Assasin";
	this->stats = {
		.hp = 50,
		.maxHp = 50,
		.damage = 30,
		.armor = 10,
		.movementSpeed = 60,
		.attackSpeed = 40
	};
}

void Assasin::specialAbility() const {}