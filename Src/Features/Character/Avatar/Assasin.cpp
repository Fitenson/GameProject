#include "Engine/Graphics.h"
#include "Assasin.h"

Assasin::Assasin() {
	this->role = "Assasin";
	this->stats = {
		.hp = 75,
		.maxHp = 75,
		.damage = 60,
		.armor = 10,
		.movementSpeed = 80,
		.attackSpeed = 35
	};
}


AvatarLook Assasin::getLook() const
{
	AvatarLook look;
	look.uniform = { 58, 44, 78, 255 };
	look.dark = { 34, 26, 48, 255 };
	look.armor = { 70, 60, 90, 255 };
	look.helmet = { 45, 34, 64, 255 };
	look.highlight = { 90, 70, 120, 255 };
	look.accent = { 205, 70, 115, 255 };
	look.pouch = { 90, 75, 115, 255 };
	look.outline = { 18, 12, 28, 255 };
	look.bulk = 0.9f;
	look.backpack = false;
	look.scarf = true;
	return look;
}
