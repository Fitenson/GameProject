#include <cmath>
#include "SwordMount.h"

SwordMount::SwordMount(const Vector2& playerPosition, float angle, float offset) 
	: playerPosition(playerPosition), angle(angle), offset(offset) {}

float& SwordMount::getOffset() { return offset; }
const float& SwordMount::getOffset() const { return offset; }

float& SwordMount::getAngle() { return angle; }
const float& SwordMount::getAngle() const { return angle; }

Vector2 SwordMount::pointAt(float distance, float sideways = 0.0f) const {
	const float c = std::cos(angle), s = std::sin(angle);
	return Vector2{
		playerPosition.x + c * distance - s * sideways,
		playerPosition.y + s * distance + c * sideways
	};
}