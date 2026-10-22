#pragma once

#include <raylib.h>

// Where a part sits while the sword swings. `angle` is the blade's current angle.
class SwordMount {
public:
	SwordMount(const Vector2& playerPosition, float angle, float offset);
	~SwordMount() = default;

	float& getOffset();
	const float& getOffset() const;

	float& getAngle();
	const float& getAngle() const;

	Vector2 pointAt(float distance, float sideways) const;

private:
	Vector2 playerPosition;
	float angle;
	float offset;   // distance from the holder to where this part begins
};