#include "Grip.h"

float Grip::length() const { return 10.0f; }

void Grip::render(Engine::Graphics::Renderer& renderer, const SwordMount& mount) const
{
	segment(renderer, mount, mount.getOffset(), mount.getOffset() + length(), 6.0f, BROWN);
}