#include "Pommel.h"

float Pommel::length() const { return 4.0f; }

void Pommel::render(Engine::Graphics::Renderer& renderer, const SwordMount& mount) const
{
	renderer.drawCircle(mount.pointAt(mount.getOffset() + 2.0f, 0.0f), 3.5f, Color{ 170, 140, 60, 255 });
}