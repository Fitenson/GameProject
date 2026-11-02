#include <algorithm>
#include <cmath>
#include <utility>
#include "Sword.h"
#include "./Attachment/Pommel.h"
#include "./Attachment/Blade.h"
#include "./Attachment/Guard.h"
#include "./Attachment/Grip.h"
#include "./Attachment/Serrations.h"
#include "./Attachment/Crescent.h"
#include "./Attachment/Fuller.h"

Sword::Sword() : Weapon("Sword", 0.28f, 1.2f, true)
{
	assemble();
}

Sword::~Sword()
{
	parts.clear();
}

// Physical order: pommel, grip, guard, blade, then the attachments that sit on the blade.
void Sword::assemble()
{
	parts.clear();

	addPart(new Pommel());
	addPart(new Grip());
	addPart(new Guard());
	addPart(new Blade());
	if (hasAttachment(SwordAttachment::Serrations)) addPart(new Serrations());
	if (hasAttachment(SwordAttachment::Fuller))     addPart(new Fuller());
	if (hasAttachment(SwordAttachment::Crescent))   addPart(new Crescent());
}

bool Sword::hasAttachment(SwordAttachment attachment) const
{
	return installed[indexOf(attachment)];
}

int Sword::getAttachmentCost(SwordAttachment attachment) const
{
	switch (attachment) {
	case SwordAttachment::Serrations: return 60;
	case SwordAttachment::Fuller:     return 50;
	case SwordAttachment::Crescent:   return 70;
	default:                          return 0;
	}
}

bool Sword::setAttachment(SwordAttachment attachment, bool value)
{
	if (hasAttachment(attachment) == value) return true;

	installed[indexOf(attachment)] = value;
	assemble();
	return true;
}

Sword::ListIterator Sword::begin() const { return parts.getIterator(); }
Sword::ListIterator Sword::end() const { return parts.getIterator().end(); }

void Sword::addPart(SwordPart* part)
{
	if (part == nullptr) return;
	if (!parts.pushBack(part)) return;
	rebuild();
}

void Sword::rebuild()
{
	stats = SwordStats{};
	tipOffset = HolderGap;

	ListIterator iterator = begin();

	while (iterator != end()) {
		SwordPart* part = iterator.getCurrent()->getValue();

		if (part != nullptr) {
			part->modify(stats);
			tipOffset += part->length();
		}

		++iterator;
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
	return std::atan2(direction.y, direction.x) + hand.side * RestSpread;
}

void Sword::onAttack(const AttackContext& context)
{
	Hand& hand = hands[nextHand];
	nextHand = (nextHand + 1) % hands.size();

	hand.centerAngle = std::atan2(context.direction.y, context.direction.x);
	hand.swingTime = stats.swingDuration;
	hand.origin = playerHand(context.playerPosition, context.direction, hand.side);

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

HandPose Sword::getHandPose(const Vector2& playerPosition, const Vector2& direction) const
{
	HandPose pose;

	for (const Hand& hand : hands) {
		const Vector2 origin = playerHand(playerPosition, direction, hand.side);
		const float angle = handAngle(hand, direction);
		const Vector2 grip{ origin.x + std::cos(angle) * GripDistance, origin.y + std::sin(angle) * GripDistance };

		if (hand.side > 0.0f) { pose.hasRight = true; pose.right = grip; }
		else { pose.hasLeft = true; pose.left = grip; }
	}
	return pose;
}

void Sword::renderHeld(Engine::Graphics::Renderer& renderer, const Vector2& playerPosition, const Vector2& direction) const
{
	for (const Hand& hand : hands) {
		const Vector2 origin = playerHand(playerPosition, direction, hand.side);
		const float angle = handAngle(hand, direction);
		float offset = HolderGap;

		ListIterator iterator = begin();

		while (iterator != end()) {
			SwordPart* part = iterator.getCurrent()->getValue();

			if (part != nullptr) {
				part->render(renderer, SwordMount{ origin, angle, offset });
				offset += part->length();
			}
			++iterator;
		}
	}
}

void Sword::renderEffects(Engine::Graphics::Renderer& renderer) const
{
	for (const Hand& hand : hands) {
		if (hand.swingTime <= 0.0f) continue;

		const float startAngle = hand.centerAngle + hand.side * stats.halfArc;
		const float nowAngle = swingAngle(hand);

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