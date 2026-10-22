#include "GunMount.h"

GunMount::GunMount(const Vector2& playerPosition, const Vector2& aimDirection, float offset, float recoil)
	: playerPosition(playerPosition), aimDirection(aimDirection), offset(offset), recoil(recoil) {
}

const Vector2& GunMount::getPlayerPosition() const { return playerPosition; }

const Vector2& GunMount::getAimDirection() const { return aimDirection; }

float GunMount::getOffset() const { return offset; }

float GunMount::getRecoil() const { return recoil; }

Vector2 GunMount::pointAt(float distance, float sideways) const
{
	const float along = distance - recoil;
	return Vector2{
		playerPosition.x + aimDirection.x * along - aimDirection.y * sideways,
		playerPosition.y + aimDirection.y * along + aimDirection.x * sideways
	};
}