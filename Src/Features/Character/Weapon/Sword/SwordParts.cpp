#include "SwordParts.h"

float Pommel::length() const { return 4.0f; }
void Pommel::render(Engine::Graphics::Renderer& renderer, const SwordMount& mount) const
{
	renderer.drawCircle(mount.pointAt(mount.getOffset() + 2.0f, 0.0f), 3.5f, Color{170, 140, 60, 255});
}

float Grip::length() const { return 10.0f; }
void Grip::render(Engine::Graphics::Renderer& renderer, const SwordMount& mount) const
{
	segment(renderer, mount, mount.getOffset(), mount.getOffset() + length(), 6.0f, BROWN);
}

float Guard::length() const { return 3.0f; }
void Guard::render(Engine::Graphics::Renderer& renderer, const SwordMount& mount) const
{
	const float direction = mount.getOffset() + 1.5f;
	renderer.drawLine(mount.pointAt(direction, -8.0f), mount.pointAt(direction, 8.0f), 4.0f, Color{ 170, 140, 60, 255 });
}

Blade::Blade(float length, float sharpness) : bladeLength(length), sharpness(sharpness) {}
float Blade::length() const { return bladeLength; }
void Blade::modify(SwordStats& s) const
{
	s.damageScale *= sharpness;
	s.swingDuration += (bladeLength - 52.0f) * 0.002f;   // longer = slower swing
}
void Blade::render(Engine::Graphics::Renderer& renderer, const SwordMount& mount) const
{
	segment(renderer, mount, mount.getOffset(), mount.getOffset() + bladeLength, 5.0f, LIGHTGRAY);
	segment(renderer, mount, mount.getOffset() + 2.0f, mount.getOffset() + bladeLength - 4.0f, 1.5f, WHITE);   // highlight
}