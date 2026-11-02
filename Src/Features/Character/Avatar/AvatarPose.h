#pragma once

#include "../Combat/HandPose.h"

struct AvatarPose {
	Vector2 position;
	Vector2 direction;
	float stride = 0.0f;
	HandPose hands;
};