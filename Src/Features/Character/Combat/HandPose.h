#pragma once

#include <raylib.h>

struct HandPose {
	bool hasRight = false;
	bool hasLeft = false;
	Vector2 right{ 0.0f, 0.0f };
	Vector2 left{ 0.0f, 0.0f };
};