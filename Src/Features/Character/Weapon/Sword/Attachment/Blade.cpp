#include "Blade.h"

Blade::Blade(float length, float sharpness) : bladeLength(length), sharpness(sharpness) {}

float Blade::length() const { return bladeLength; }

void Blade::modify(SwordStats& stats) const
{
	stats.damageScale *= sharpness;
	stats.swingDuration += (bladeLength - 52.0f) * 0.002f;   // longer = slower swing
}

void Blade::render(Engine::Graphics::Renderer& renderer, const SwordMount& mount) const
{
	segment(renderer, mount, mount.getOffset(), mount.getOffset() + bladeLength, 5.0f, LIGHTGRAY);
	segment(renderer, mount, mount.getOffset() + 2.0f, mount.getOffset() + bladeLength - 4.0f, 1.5f, WHITE);   // highlight
}