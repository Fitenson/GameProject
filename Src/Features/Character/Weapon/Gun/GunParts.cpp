#include "GunParts.h"

// ---------- Receiver ----------
float Receiver::length() const { return 14.0f; }
void Receiver::render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const
{
	drawSegment(renderer, mount, mount.getOffset(), mount.getOffset() + length(), 9.0f, Color{ 60, 60, 70, 255 });
}

// ---------- Barrel ----------
Barrel::Barrel(float length) : barrelLength(length) {}
float Barrel::length() const { return barrelLength; }
void Barrel::modify(GunStats& stats) const
{
	// Longer than the 18px standard barrel gives a little extra speed and range.
	const float bonus = barrelLength - 18.0f;
	stats.bulletSpeed += bonus * 8.0f;
	stats.bulletRange += bonus * 6.0f;
}
void Barrel::render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const
{
	drawSegment(renderer, mount, mount.getOffset(), mount.getOffset() + barrelLength, 5.0f, DARKGRAY);
}

// ---------- Scope ----------
Scope::Scope(float extraRange) : extraRange(extraRange) {}
float Scope::length() const { return 0.0f; }
void Scope::modify(GunStats& stats) const { stats.bulletRange += extraRange; }
void Scope::render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const
{
	drawSegment(renderer, mount, mount.getOffset(), mount.getOffset() + 12.0f, 4.0f, Color{ 30, 30, 50, 255 }, 6.0f);
}

// ---------- Suppressor ----------
float Suppressor::length() const { return 14.0f; }
void Suppressor::modify(GunStats& stats) const
{
	stats.flashScale = 0.0f;
	stats.bulletSpeed *= 0.95f;
}
void Suppressor::render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const
{
	drawSegment(renderer, mount, mount.getOffset(), mount.getOffset() + length(), 8.0f, Color{ 25, 25, 30, 255 });
}

// ---------- Compensator ----------
float Compensator::length() const { return 6.0f; }
void Compensator::modify(GunStats& stats) const { stats.recoil *= 0.5f; }
void Compensator::render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const
{
	drawSegment(renderer, mount, mount.getOffset(), mount.getOffset() + length(), 8.0f, Color{ 90, 90, 100, 255 });
}

// ---------- HeavyReceiver ----------
float HeavyReceiver::length() const { return 20.0f; }
void HeavyReceiver::render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const
{
	drawSegment(renderer, mount, mount.getOffset(), mount.getOffset() + length(), 13.0f, Color{ 55, 60, 75, 255 });
	drawSegment(renderer, mount, mount.getOffset() + 3.0f, mount.getOffset() + length() - 3.0f, 5.0f, Color{ 90, 100, 120, 255 });
}

// ---------- HeavyBarrel ----------
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