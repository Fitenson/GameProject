#include "Suppressor.h"

float Suppressor::length() const { return 14.0f; }

void Suppressor::modify(GunStats& stats) const
{
	stats.flashScale = 0.0f;
	stats.bulletSpeed *= 0.95f;
}
void Suppressor::render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const
{
	drawSegment(renderer, mount, mount.getOffset(), mount.getOffset() + length(), 8.0f, Color{ 25, 25, 30, 255 });
}