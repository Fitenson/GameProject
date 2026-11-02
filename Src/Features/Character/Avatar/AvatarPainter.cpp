#include <algorithm>
#include <cmath>
#include "AvatarPainter.h"

namespace {
	using Engine::Graphics::Renderer;

	constexpr float ArmSegment = 10.0f;   // upper arm and forearm length

	// Local axes: forward = aim direction, side = the character's right-hand side.
	struct Frame {
		Vector2 origin, forward, side;

		Vector2 at(float a, float b) const {
			return { origin.x + forward.x * a + side.x * b, origin.y + forward.y * a + side.y * b };
		}
	};

	Frame frameOf(const AvatarPose& pose)
	{
		return { pose.position, pose.direction, Vector2{ -pose.direction.y, pose.direction.x } };
	}

	// A thick bar across the body, centred at forward distance `a`.
	void bar(Renderer& r, const Frame& f, float a, float half, float depth, Color color)
	{
		r.drawLine(f.at(a, -half), f.at(a, half), depth, color);
	}

	// Two-segment arm. The elbow bends outward; a fully stretched arm is straight.
	void drawArm(Renderer& r, Vector2 shoulder, Vector2 hand, Vector2 outward, const AvatarLook& look)
	{
		const Vector2 d{ hand.x - shoulder.x, hand.y - shoulder.y };
		const float distance = std::sqrt(d.x * d.x + d.y * d.y);

		Vector2 elbow = shoulder;
		if (distance > 0.001f) {
			const Vector2 u{ d.x / distance, d.y / distance };
			Vector2 n{ -u.y, u.x };
			if (n.x * outward.x + n.y * outward.y < 0.0f) n = { -n.x, -n.y };

			const float bend = std::sqrt(std::max(0.0f, ArmSegment * ArmSegment - distance * distance * 0.25f));
			elbow = { shoulder.x + d.x * 0.5f + n.x * bend, shoulder.y + d.y * 0.5f + n.y * bend };
		}

		// Outline first, then the sleeve, forearm and glove.
		r.drawLine(shoulder, elbow, 6.5f, look.outline);
		r.drawLine(elbow, hand, 6.0f, look.outline);
		r.drawLine(shoulder, elbow, 5.0f, look.uniform);
		r.drawCircle(elbow, 2.6f, look.uniform);
		r.drawLine(elbow, hand, 4.5f, look.dark);
		r.drawCircle(hand, 3.4f, look.outline);
		r.drawCircle(hand, 2.6f, look.glove);
	}
}

void AvatarPainter::drawBody(Renderer& r, const AvatarPose& pose, const AvatarLook& look)
{
	const Frame f = frameOf(pose);
	const float k = look.bulk;

	// Shadow
	r.drawCircle(Vector2{ pose.position.x + 1.5f, pose.position.y + 2.0f }, 12.0f * k, Color{ 0, 0, 0, 65 });

	// Legs: feet swing forward and back with the stride.
	for (float side : { -1.0f, 1.0f }) {
		const Vector2 hip = f.at(-1.0f, side * 3.5f * k);
		const Vector2 foot = f.at(1.0f + side * pose.stride * 4.5f, side * 3.5f * k);
		r.drawLine(hip, foot, 5.0f * k, look.dark);
		r.drawCircle(foot, 3.2f * k, look.boots);
	}

	// Backpack
	if (look.backpack) {
		bar(r, f, -7.0f * k, 4.5f * k, 6.0f * k + 2.0f, look.outline);
		bar(r, f, -7.0f * k, 4.5f * k, 6.0f * k, look.dark);
		bar(r, f, -8.5f * k, 3.0f * k, 1.5f, look.pouch);
	}

	// Torso with round shoulders
	const float half = 7.0f * k;
	const float depth = 9.0f * k;
	bar(r, f, 0.0f, half, depth + 2.0f, look.outline);
	for (float side : { -1.0f, 1.0f }) r.drawCircle(f.at(0.0f, side * half), depth * 0.5f + 1.0f, look.outline);
	bar(r, f, 0.0f, half, depth, look.uniform);
	for (float side : { -1.0f, 1.0f }) r.drawCircle(f.at(0.0f, side * half), depth * 0.5f, look.uniform);

	// Vest, centre strip and pouches
	bar(r, f, 0.5f, 5.0f * k, 6.0f * k, look.armor);
	r.drawLine(f.at(-2.5f, 0.0f), f.at(3.0f, 0.0f), 1.5f, look.dark);
	for (float side : { -1.0f, 1.0f }) r.drawCircle(f.at(3.0f, side * 3.2f * k), 1.6f, look.pouch);

	// Heavy shoulder pads
	if (look.shoulderPads) {
		for (float side : { -1.0f, 1.0f }) {
			const Vector2 pad = f.at(0.0f, side * (half + 1.5f));
			r.drawCircle(pad, 6.0f * k, look.outline);
			r.drawCircle(pad, 5.0f * k, look.armor);
			r.drawCircle(f.at(-1.0f, side * (half + 0.5f)), 1.8f, look.highlight);
		}
	}
}

void AvatarPainter::drawUpper(Renderer& renderer, const AvatarPose& pose, const AvatarLook& look)
{
	const Frame f = frameOf(pose);
	const float k = look.bulk;
	const float shoulder = 7.0f * k;

	// Arms reach for the weapon. Without one, they hang at the sides.
	const Vector2 handRight = pose.hands.hasRight ? pose.hands.right : f.at(4.0f, 9.5f * k);
	const Vector2 handLeft = pose.hands.hasLeft ? pose.hands.left : f.at(4.0f, -9.5f * k);

	drawArm(renderer, f.at(0.0f, shoulder), handRight, f.side, look);
	drawArm(renderer, f.at(0.0f, -shoulder), handLeft, Vector2{ -f.side.x, -f.side.y }, look);

	// Scarf trailing behind the head
	if (look.scarf) {
		const float sway = std::sin(static_cast<float>(GetTime()) * 6.0f) * 2.0f + pose.stride * 1.5f;
		const Vector2 a = f.at(-4.0f, 0.0f), b = f.at(-10.0f, sway), c = f.at(-15.0f, sway * 1.8f);
		renderer.drawLine(a, b, 3.5f, look.accent);
		renderer.drawLine(b, c, 2.5f, look.accent);
	}

	// Helmet seen from above
	const Vector2 head = f.at(1.0f, 0.0f);
	renderer.drawCircle(head, 6.5f * k, look.outline);
	renderer.drawCircle(head, 5.5f * k, look.helmet);
	renderer.drawCircle(f.at(2.5f, -2.0f * k), 2.0f * k, look.highlight);
	bar(renderer, f, 5.0f * k, 2.6f * k, 1.6f, look.accent);   // front marker shows where the avatar faces
}