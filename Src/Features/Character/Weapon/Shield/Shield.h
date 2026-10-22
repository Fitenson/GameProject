#pragma once

#include <raylib.h>
#include <algorithm>
#include "Engine/Graphics.h"

class Shield {
public:
	Shield(float maxHp);

	void update(float deltaTime);

	// Absorbs as much of damage as it can and returns what is left over.
	float absorb(float damage);

	//	Checking if the shield have health left
	bool isActive() const;
	bool isRegenerating() const;
	float getHp() const;
	float getMaxHp() const;

	void render(Engine::Graphics::Renderer& renderer, const Vector2& center, float radius) const;
	void renderBar(Engine::Graphics::Renderer& renderer, const Rectangle& area) const;

private:
	// Delay seconds to control when to start repairing the shield
	static constexpr float RegenDelay = 5.0f;
	// of max hp per second
	static constexpr float RegenFraction = 0.15f;
	static constexpr float HitFlashDuration = 0.15f;
	static constexpr float TwoPi = 6.2831853f;

	float maxHp;
	float hp;
	float sinceDamage = RegenDelay;
	float hitFlash = 0.0f;
	// Drives the pulse and the rotating arcs
	float clock = 0.0f;

	static unsigned char byteOf(float value)
	{
		return static_cast<unsigned char>(std::clamp(value, 0.0f, 255.0f));
	}
};