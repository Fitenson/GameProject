#include "HeavyGun.h"
#include "GunParts.h"

HeavyGun::HeavyGun() : Gun("Heavy Gun", 0.75f, 2.0f)   // 3x the damage at 0.45s per shot
{
	setMovementMultiplier(0.3f);   // 40% speed while firing

	addPart(makeUnique<HeavyReceiver>());
	addPart(makeUnique<HeavyBarrel>());
}