#include "Trooper.h"

Trooper::Trooper() {
	this->role = "Trooper";
	this->stats = {
		.hp = 80,
		.maxHp = 80,
		.damage = 12,
		.armor = 16,
		.movementSpeed = 30,
		.attackSpeed = 16
	};
}