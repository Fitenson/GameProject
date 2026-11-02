#include "Scope.h"

Scope::Scope(float extraRange) : extraRange(extraRange) {}

float Scope::length() const { return 0.0f; }
void Scope::modify(GunStats& stats) const { stats.bulletRange += extraRange; }
void Scope::render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const
{
	drawSegment(renderer, mount, mount.getOffset(), mount.getOffset() + 12.0f, 4.0f, Color{ 30, 30, 50, 255 }, 6.0f);
}