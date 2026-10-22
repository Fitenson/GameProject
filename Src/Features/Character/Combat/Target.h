#pragma once

// A snapshot of something enemies can attack, valid for one frame.
struct Target {
	Vector2 position;
	float radius;
	bool alive = true;
	float damage = 0.0f;
};