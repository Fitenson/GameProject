#include "Bullet.h"

Bullet::Bullet(const Vector2& position, const Vector2& direction, float speed, float damage, float maxDistance)
	: Entity(position), direction(direction), speed(speed), damage(damage), remainingDistance(maxDistance) {
}

void Bullet::update(float deltaTime)
{
	const float step = speed * deltaTime;
	position.x += direction.x * step;
	position.y += direction.y * step;

	remainingDistance -= step;
	if (remainingDistance <= 0.0f) alive = false;
}

void Bullet::render(Engine::Graphics::Renderer& renderer) const
{
	// Tracer: a short streak behind the bullet head.
	constexpr float tracerLength = 18.0f;
	const Vector2 tail{ position.x - direction.x * tracerLength, position.y - direction.y * tracerLength };

	renderer.drawLine(tail, position, 3.0f, Color{ 255, 220, 80, 255 });
	renderer.drawCircle(position, radius, Color{ 255, 255, 200, 255 });
}

float Bullet::getDamage() const { return damage; }
bool Bullet::isAlive() const { return alive; }
void Bullet::kill() { alive = false; }