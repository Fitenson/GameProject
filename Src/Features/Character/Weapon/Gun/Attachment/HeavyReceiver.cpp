#include "HeavyReceiver.h"

float HeavyReceiver::length() const { return 20.0f; }
void HeavyReceiver::render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const
{
	drawSegment(renderer, mount, mount.getOffset(), mount.getOffset() + length(), 13.0f, Color{ 55, 60, 75, 255 });
	drawSegment(renderer, mount, mount.getOffset() + 3.0f, mount.getOffset() + length() - 3.0f, 5.0f, Color{ 90, 100, 120, 255 });
}