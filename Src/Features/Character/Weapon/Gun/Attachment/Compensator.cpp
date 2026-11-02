#include "Compensator.h"

float Compensator::length() const { return 6.0f; }
void Compensator::modify(GunStats& stats) const { stats.recoil *= 0.5f; }
void Compensator::render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const
{
	drawSegment(renderer, mount, mount.getOffset(), mount.getOffset() + length(), 8.0f, Color{ 90, 90, 100, 255 });
}