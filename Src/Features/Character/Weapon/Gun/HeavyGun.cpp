#include "HeavyGun.h"
#include "./Attachment/HeavyReceiver.h"
#include "./Attachment/HeavyBarrel.h"

HeavyGun::HeavyGun() : Gun("Heavy Gun", 0.75f, 2.0f, false)   // 3x the damage at 0.45s per shot
{
	setMovementMultiplier(0.3f);   // 40% speed while firing

	addPart(new HeavyReceiver());
	addPart(new HeavyBarrel());
}