#include "Fuller.h"

float Fuller::length() const { return 0.0f; }

void Fuller::modify(SwordStats& stats) const
{
	stats.swingDuration = std::max(0.08f, stats.swingDuration - 0.04f);
}

void Fuller::render(Engine::Graphics::Renderer& renderer, const SwordMount& mount) const
{
	const float tip = mount.getOffset();
	renderer.drawLine(mount.pointAt(tip - 46.0f, 0.0f), mount.pointAt(tip - 6.0f, 0.0f), 1.5f, Color{ 90, 110, 150, 255 });
}