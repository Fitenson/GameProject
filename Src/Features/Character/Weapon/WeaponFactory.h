#pragma once

#include "Engine/Core.h"
#include "Weapon.h"

enum WeaponType { Gun, Sword, HeavyGun };

class WeaponFactory
{
public:
	static UniquePtr<Weapon> create(WeaponType type);
};