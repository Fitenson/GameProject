#include "Crescent.h"

float Crescent::length() const { return 0.0f; }

void Crescent::modify(SwordStats& stats) const { stats.halfArc += 0.25f; }

void Crescent::render(Engine::Graphics::Renderer& renderer, const SwordMount& mount) const
{
	const float tip = mount.getOffset();
	const Color gold{ 245, 200, 60, 255 };
	renderer.drawLine(mount.pointAt(tip - 8.0f, 0.0f), mount.pointAt(tip - 2.0f, 4.5f), 2.5f, gold);
	renderer.drawLine(mount.pointAt(tip - 2.0f, 4.5f), mount.pointAt(tip + 3.0f, 2.0f), 2.5f, gold);
}