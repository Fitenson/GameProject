#include "Receiver.h"

float Receiver::length() const { return 14.0f; }

void Receiver::render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const
{
	drawSegment(renderer, mount, mount.getOffset(), mount.getOffset() + length(), 9.0f, Color{ 60, 60, 70, 255 });
}