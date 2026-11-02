#include "Guard.h"

float Guard::length() const { return 3.0f; }

void Guard::render(Engine::Graphics::Renderer& renderer, const SwordMount& mount) const
{
	const float direction = mount.getOffset() + 1.5f;
	renderer.drawLine(mount.pointAt(direction, -8.0f), mount.pointAt(direction, 8.0f), 4.0f, Color{ 170, 140, 60, 255 });
}