#include "WeaponFactory.h"
#include "Gun/Gun.h"
#include "Sword/Sword.h"
#include "Gun/HeavyGun.h"

UniquePtr<Weapon> WeaponFactory::create(WeaponType type) {
	switch (type) {
		case WeaponType::Gun:
			return makeUnique<Gun>();
		case WeaponType::Sword:
			return makeUnique<Sword>();
		case WeaponType::HeavyGun:
			return makeUnique<HeavyGun>();
	}
	return nullptr;
}