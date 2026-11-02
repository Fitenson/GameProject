#pragma once

#include <vector>
#include <raylib.h>
#include "Engine/Core.h"
#include "../../Monster/Enemy.h"

struct AttackContext {
	Vector2 playerPosition;
	Vector2 direction;
	float baseDamage;
	const std::vector<Enemy*>& targets;
	bool reloadRequested = false;
	float rateMultiplier = 1.0f;
	bool unlimitedAmmo = false;
};