#include "Engine/Graphics.h"
#include "Soldier.h"

Soldier::Soldier() {
	this->role = "Soldier";
	this->stats = {
		.hp = 100,
		.maxHp = 100,
		.damage = 30,
		.armor = 20,
		.movementSpeed = 60,
		.attackSpeed = 25
	};
}

std::string Soldier::getAbilityName() const { return "Overdrive"; }