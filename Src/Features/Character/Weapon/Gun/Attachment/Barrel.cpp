#include "Barrel.h"

Barrel::Barrel(float length) : barrelLength(length) {}

float Barrel::length() const { return barrelLength; }

void Barrel::modify(GunStats& stats) const
{
	// Longer than the 18px standard barrel gives a little extra speed and range.
	const float bonus = barrelLength - 18.0f;
	stats.bulletSpeed += bonus * 8.0f;
	stats.bulletRange += bonus * 6.0f;
}

void Barrel::render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const
{
	drawSegment(renderer, mount, mount.getOffset(), mount.getOffset() + barrelLength, 5.0f, DARKGRAY);
}