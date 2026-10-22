#include <cmath>
#include <utility>
#include "Ally.h"

Ally::Ally(Character character, const Vector2& position)
	: Entity(position), character(std::move(character)), leader(position) {
}

void Ally::setLeader(const Vector2& leaderPosition) { leader = leaderPosition; }

Vector2 Ally::getHitPosition() const { return position; }

float Ally::getHitRadius() const { return BodyRadius; }

bool Ally::isDead() const { return character.isDead(); }

void Ally::update(float deltaTime)
{
	const float dx = leader.x - position.x;
	const float dy = leader.y - position.y;
	const float distance = std::sqrt(dx * dx + dy * dy);

	// Stuck behind trees? Catch up instead of getting lost.
	if (distance > TeleportDistance) {
		position = Vector2{ leader.x - 20.0f, leader.y };
		return;
	}
	if (distance <= FollowDistance) return;

	const float speed = character.getMovementSpeed() * 4.0f * 0.9f;   // a little slower than the player
	position.x += dx / distance * speed * deltaTime;
	position.y += dy / distance * speed * deltaTime;
}

void Ally::updateCombat(float deltaTime, const std::vector<Enemy*>& targets)
{
	if (isDead()) return;
	invulnerableTimer = std::max(0.0f, invulnerableTimer - deltaTime);

	character.update(deltaTime);

	// Aim at the nearest living enemy in range.
	const Enemy* nearest = nullptr;
	float best = FireRange * FireRange;

	for (const Enemy* enemy : targets) {
		if (enemy->isDead()) continue;
		const Vector2 p = enemy->getHitPosition();
		const float dx = p.x - position.x;
		const float dy = p.y - position.y;
		const float d = dx * dx + dy * dy;
		if (d < best) { best = d; nearest = enemy; }
	}

	bool shooting = false;
	if (nearest) {
		const Vector2 p = nearest->getHitPosition();
		const float dx = p.x - position.x;
		const float dy = p.y - position.y;
		const float length = std::sqrt(dx * dx + dy * dy);
		if (length > 1.0f) {
			aimDirection = Vector2{ dx / length, dy / length };
			shooting = true;
		}
	}

	const AttackContext context{ position, aimDirection, character.getDamage(), targets };
	character.updateWeapons(deltaTime, context, false, shooting);
}

void Ally::render(Engine::Graphics::Renderer& renderer) const
{
	const bool blink = invulnerableTimer > 0.0f && (static_cast<int>(invulnerableTimer * 30.0f) % 2 == 0);
	renderer.drawCircle(position, BodyRadius, blink ? RED : Color{ 40, 170, 170, 255 });
	renderer.drawCircleOutline(position, BodyRadius, WHITE);

	if (character.getHp() < character.getMaxHp()) {
		constexpr float barWidth = 28.0f;
		const float ratio = std::clamp(character.getHp() / character.getMaxHp(), 0.0f, 1.0f);
		const Rectangle back{ position.x - barWidth * 0.5f, position.y - BodyRadius - 10.0f, barWidth, 4.0f };
		renderer.drawRectangle(back, Color{ 20, 20, 20, 200 });
		renderer.drawRectangle(Rectangle{ back.x, back.y, back.width * ratio, back.height }, Color{ 80, 200, 80, 255 });
		renderer.drawRectangleOutline(back, 1.0f, BLACK);
	}

	character.renderWeaponEffects(renderer);
	character.renderHeldWeapon(renderer, position, aimDirection);
}

void Ally::takeDamage(float amount)
{
	if (isDead() || invulnerableTimer > 0.0f) return;
	character.takeDamage(amount);   // armor applies
	invulnerableTimer = InvulnerableTime;
}