#include "HeavyGun.h"
#include "./Attachment/HeavyReceiver.h"
#include "./Attachment/HeavyBarrel.h"

HeavyGun::HeavyGun() : Gun("Heavy Gun", 0.75f, 2.0f, false)   // 2x the damage at 0.75s per shot
{
	setMovementMultiplier(0.3f);   // 30% speed while firing

	addPart(new Magazine(30.0f, 8, 0.6f));
	addPart(new HeavyReceiver());
	addPart(new HeavyBarrel());
}