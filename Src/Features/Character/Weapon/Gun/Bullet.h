#pragma once

#include <raylib.h>
#include "Engine/Graphics.h"
#include "../../../Game/Entity.h"

class Bullet : public Entity {
public:
	Bullet(const Vector2& position, const Vector2& direction, float speed, float damage, float maxDistance);

	static constexpr float radius = 3.0f;

	void update(float deltaTime) override;
	void render(Engine::Graphics::Renderer& renderer) const override;

	float getDamage() const;
	bool isAlive() const;
	void kill();

private:
	Vector2 direction;
	float speed;
	float damage;
	float remainingDistance;
	bool alive = true;
};