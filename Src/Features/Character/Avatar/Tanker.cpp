#include "Engine/Graphics.h"
#include "Tanker.h"

Tanker::Tanker() {
	this->role = "Tanker";
	this->stats = {
		.hp = 200,
		.maxHp = 200,
		.damage = 40,
		.armor = 30,
		.movementSpeed = 40,
		.attackSpeed = 15
	};
}

AvatarLook Tanker::getLook() const
{
	AvatarLook look;
	look.uniform = { 150, 95, 45, 255 };
	look.dark = { 95, 60, 30, 255 };
	look.armor = { 120, 125, 135, 255 };
	look.helmet = { 100, 105, 115, 255 };
	look.highlight = { 165, 170, 180, 255 };
	look.accent = { 235, 170, 50, 255 };
	look.outline = { 35, 25, 18, 255 };
	look.bulk = 1.3f;
	look.shoulderPads = true;
	return look;
}

std::string Tanker::getAbilityName() const { return "Fortify"; }

float Tanker::getShieldRatio() const { return 0.5f; }

void Tanker::specialAbility()
{
	stats.damage *= Boost;
	stats.armor *= Boost;
	stats.attackSpeed *= Boost;
}

void Tanker::endAbility()
{
	stats.damage /= Boost;
	stats.armor /= Boost;
	stats.attackSpeed /= Boost;
}