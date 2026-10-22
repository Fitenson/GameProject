#include <algorithm>
#include <cmath>
#include "Shield.h"

Shield::Shield(float maxHp) : maxHp(maxHp), hp(maxHp) {}

float Shield::getHp() const { return hp; }
float Shield::getMaxHp() const { return maxHp; }
bool Shield::isActive() const { return hp > 0.0f; }
bool Shield::isRegenerating() const { return sinceDamage >= RegenDelay && hp < maxHp; }

void Shield::update(float deltaTime)
{
	clock += deltaTime;
	hitFlash = std::max(0.0f, hitFlash - deltaTime);
	sinceDamage += deltaTime;

	if (isRegenerating()) {
		hp = std::min(maxHp, hp + maxHp * RegenFraction * deltaTime);
	}
}

float Shield::absorb(float damage)
{
	sinceDamage = 0.0f;   // the repair delay restarts on every hit

	const float absorbed = std::min(hp, damage);
	hp -= absorbed;
	if (absorbed > 0.0f) hitFlash = HitFlashDuration;

	return damage - absorbed;
}

void Shield::render(Engine::Graphics::Renderer& renderer, const Vector2& center, float radius) const
{
	if (!isActive()) return;

	const float ratio = std::clamp(hp / maxHp, 0.0f, 1.0f);
	const float pulse = 0.5f + 0.5f * std::sin(clock * 3.0f);
	const float flash = hitFlash / HitFlashDuration;                 // 1 -> 0 after a hit
	const float strength = 0.5f + 0.5f * ratio;                      // weaker shield looks fainter
	const float flicker = (ratio < 0.25f && std::fmod(clock * 10.0f, 1.0f) < 0.5f) ? 0.4f : 1.0f;

	// Soft translucent bubble.
	const float fill = (35.0f + 25.0f * pulse + 90.0f * flash) * strength * flicker;
	renderer.drawCircle(center, radius, Color{ 60, 150, 255, byteOf(fill) });

	// Double rim.
	const float rim = (170.0f + 60.0f * flash) * flicker;
	renderer.drawCircleOutline(center, radius, Color{ 130, 210, 255, byteOf(rim) });
	renderer.drawCircleOutline(center, radius - 2.0f, Color{ 90, 170, 255, byteOf(rim * 0.5f) });

	// Three bright arcs sweeping around the rim.
	constexpr int arcs = 3;
	constexpr int steps = 5;
	constexpr float arcSpan = 0.7f;
	for (int a = 0; a < arcs; ++a) {
		const float start = clock * 1.5f + a * (TwoPi / arcs);
		for (int s = 0; s < steps; ++s) {
			const float a0 = start + arcSpan * s / steps;
			const float a1 = start + arcSpan * (s + 1) / steps;
			const float fade = static_cast<float>(s + 1) / steps;   // brighter at the leading end
			renderer.drawLine(
				Vector2{ center.x + std::cos(a0) * radius, center.y + std::sin(a0) * radius },
				Vector2{ center.x + std::cos(a1) * radius, center.y + std::sin(a1) * radius },
				3.0f,
				Color{ 200, 240, 255, byteOf(230.0f * fade * strength * flicker) });
		}
	}
}

void Shield::renderBar(Engine::Graphics::Renderer& renderer, const Rectangle& area) const
{
	const float ratio = std::clamp(hp / maxHp, 0.0f, 1.0f);

	renderer.drawRectangle(area, Color{ 15, 20, 35, 210 });

	// Cyan while repairing, blue otherwise.
	const Color fill = isRegenerating() ? Color{ 110, 235, 255, 255 } : Color{ 60, 140, 255, 255 };
	renderer.drawRectangle(Rectangle{ area.x, area.y, area.width * ratio, area.height }, fill);
	renderer.drawRectangleOutline(area, 1.0f, BLACK);

	// Thin strip under the bar: how close the repair is to starting.
	if (sinceDamage < RegenDelay && hp < maxHp) {
		const float wait = std::clamp(sinceDamage / RegenDelay, 0.0f, 1.0f);
		renderer.drawRectangle(
			Rectangle{ area.x, area.y + area.height + 1.0f, area.width * wait, 2.0f },
			Color{ 130, 200, 255, 200 });
	}
}