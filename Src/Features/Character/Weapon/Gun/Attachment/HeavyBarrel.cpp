#include "HeavyBarrel.h"

HeavyBarrel::HeavyBarrel(float length) : barrelLength(length) {}

float HeavyBarrel::length() const { return barrelLength; }
void HeavyBarrel::modify(GunStats& stats) const
{
	stats.bulletSpeed *= 0.85f;   // slower, heavier shells
	stats.bulletRange += 120.0f;
	stats.recoil *= 2.0f;
	stats.flashScale *= 1.6f;
}
void HeavyBarrel::render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const
{
	const float from = mount.getOffset();
	drawSegment(renderer, mount, from, from + barrelLength, 9.0f, DARKGRAY);

	// Cooling fins.
	for (int i = 1; i <= 3; ++i) {
		const float d = from + barrelLength * i / 4.0f;
		renderer.drawLine(mount.pointAt(d, -6.0f), mount.pointAt(d, 6.0f), 3.0f, Color{ 120, 125, 140, 255 });
	}
}