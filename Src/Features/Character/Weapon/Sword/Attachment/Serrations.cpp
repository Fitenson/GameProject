#include "Serrations.h"

float Serrations::length() const { return 0.0f; }

void Serrations::modify(SwordStats& stats) const { stats.damageScale *= 1.25f; }

void Serrations::render(Engine::Graphics::Renderer& renderer, const SwordMount& mount) const
{
	const float tip = mount.getOffset();
	for (int i = 0; i < 6; ++i) {
		const float d = tip - 10.0f - static_cast<float>(i) * 6.5f;
		for (float side : { -1.0f, 1.0f }) {
			renderer.drawLine(mount.pointAt(d, side * 2.5f), mount.pointAt(d - 2.5f, side * 5.0f), 1.5f, Color{ 215, 220, 230, 255 });
		}
	}
}
