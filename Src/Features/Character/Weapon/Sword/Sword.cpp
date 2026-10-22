#include <algorithm>
#include <cmath>
#include <utility>
#include "Sword.h"
#include "SwordParts.h"

Sword::Sword() : parts(nullptr), tail(nullptr), Weapon("Sword", 0.28f, 1.2f, true)   // each blade hits a little lighter, but faster
{
	addPart(makeUnique<Pommel>());
	addPart(makeUnique<Grip>());
	addPart(makeUnique<Guard>());
	addPart(makeUnique<Blade>());
}

void Sword::addPart(UniquePtr<SwordPart> part)
{
	if (part == nullptr) return;

	SwordPart* newPart = part.get();   // grab it before the move

	if (tail == nullptr) {
		parts = std::move(part);
	}
	else {
		tail->setNext(std::move(part));
	}

	tail = newPart;

	rebuild();
}

// Recompute the stats and the tip position by walking the chain.
void Sword::rebuild()
{
	stats = SwordStats{};
	tipOffset = HolderGap;

	for (const SwordPart* part = parts.get(); part; part = part->getNext()) {
		part->modify(stats);
		tipOffset += part->length();
	}
}

// The hand sits to the side of the player, perpendicular to the aim.
Vector2 Sword::playerHand(const Vector2& playerPosition, const Vector2& direction, float side)
{
	return Vector2{
		playerPosition.x - direction.y * side * HandOffset,
		playerPosition.y + direction.x * side * HandOffset
	};
}

// Sweeps from the outer side, across the front of the body, to the inner side.
float Sword::swingAngle(const Hand& hand) const
{
	const float progress = 1.0f - hand.swingTime / stats.swingDuration;
	return hand.centerAngle + hand.side * stats.halfArc * (1.0f - 2.0f * progress);
}

float Sword::handAngle(const Hand& hand, const Vector2& direction) const
{
	if (hand.swingTime > 0.0f) return swingAngle(hand);
	return std::atan2(direction.y, direction.x) + hand.side * RestSpread;   // idle: angled outward
}

void Sword::onAttack(const AttackContext& context)
{
	Hand& hand = hands[nextHand];
	nextHand = (nextHand + 1) % hands.size();   // the other hand swings next

	hand.centerAngle = std::atan2(context.direction.y, context.direction.x);
	hand.swingTime = stats.swingDuration;
	hand.origin = playerHand(context.playerPosition, context.direction, hand.side);

	// Damage lands instantly for responsiveness; the animation is cosmetic.
	const float damage = damageOf(context) * stats.damageScale;

	for (Enemy* target : context.targets) {
		if (target->isDead()) continue;

		if (inCone(hand.origin, context.direction, target->getHitPosition(),
			target->getHitRadius(), tipOffset, stats.halfArc)) {
			target->takeDamage(damage);
		}
	}
}

void Sword::onUpdate(float deltaTime, const AttackContext& context)
{
	for (Hand& hand : hands) {
		hand.swingTime = std::max(0.0f, hand.swingTime - deltaTime);
		hand.origin = playerHand(context.playerPosition, context.direction, hand.side);
	}
}

void Sword::renderHeld(Engine::Graphics::Renderer& renderer, const Vector2& playerPosition, const Vector2& direction) const
{
	for (const Hand& hand : hands) {
		const Vector2 origin = playerHand(playerPosition, direction, hand.side);
		const float angle = handAngle(hand, direction);

		// Each part starts where the previous one ended.
		float offset = HolderGap;
		for (const SwordPart* part = parts.get(); part; part = part->getNext()) {
			part->render(renderer, SwordMount{ origin, angle, offset });
			offset += part->length();
		}
	}
}

void Sword::renderEffects(Engine::Graphics::Renderer& renderer) const
{
	for (const Hand& hand : hands) {
		if (hand.swingTime <= 0.0f) continue;

		const float startAngle = hand.centerAngle + hand.side * stats.halfArc;
		const float nowAngle = swingAngle(hand);

		// Slash trail: fades in toward the blade's current position.
		constexpr int steps = 10;
		for (int i = 0; i < steps; ++i) {
			const float a0 = startAngle + (nowAngle - startAngle) * i / steps;
			const float a1 = startAngle + (nowAngle - startAngle) * (i + 1) / steps;
			const unsigned char alpha = static_cast<unsigned char>(40 + 200 * (i + 1) / steps);

			const Vector2 p0{ hand.origin.x + std::cos(a0) * tipOffset, hand.origin.y + std::sin(a0) * tipOffset };
			const Vector2 p1{ hand.origin.x + std::cos(a1) * tipOffset, hand.origin.y + std::sin(a1) * tipOffset };
			renderer.drawLine(p0, p1, 4.0f, Color{ 200, 230, 255, alpha });
		}
	}
}