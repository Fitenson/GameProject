#pragma once

#include <raylib.h>

class GunMount {
public:
	GunMount(const Vector2& playerPosition, const Vector2& aimDirection, float offset, float recoil);

	const Vector2& getPlayerPosition() const;
	const Vector2& getAimDirection() const;
	float getOffset() const;
	float getRecoil() const;

	// A point `distance` along the barrel axis (minus recoil), `sideways` off the axis.
	Vector2 pointAt(float distance, float sideways = 0.0f) const;

private:
	Vector2 playerPosition;
	Vector2 aimDirection;
	float offset;
	float recoil;
};