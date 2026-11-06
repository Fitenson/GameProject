// Ally.cpp
#include <algorithm>
#include <cmath>
#include <utility>
#include "Ally.h"

Ally::Ally(Character character, const Vector2& position)
	: Entity(position), character(std::move(character)), leader(position) {
}

Character& Ally::getCharacter() { return character; }

void Ally::setLeader(const Vector2& leaderPosition) { leader = leaderPosition; }

void Ally::setCombatStyle(float preferred, float engage, float attack)
{
	preferredDistance = preferred;
	engageRange = engage;
	attackRange = attack;
}

void Ally::makeTemporary(float seconds)
{
	temporary = true;
	lifetimeLeft = seconds;
}

bool Ally::isTemporary() const { return temporary; }
bool Ally::isExpired() const { return temporary && lifetimeLeft <= 0.0f; }

Vector2 Ally::getHitPosition() const { return position; }
float Ally::getHitRadius() const { return BodyRadius; }
bool Ally::isDead() const { return character.isDead(); }

void Ally::takeDamage(float amount)
{
	if (isDead() || invulnerableTimer > 0.0f) return;
	character.takeDamage(amount);
	invulnerableTimer = InvulnerableTime;
}

void Ally::restoreHealth(float fraction)
{
	character.heal(character.getMaxHp() * fraction);
}

// ---------- Movement ----------

void Ally::update(float deltaTime)
{
	strafeClock += deltaTime;

	const float speed = character.getMovementSpeed() * 4.0f * 0.9f;   // a little slower than the player

	// Lost far behind the leader: catch up instead of getting stuck.
	const float lx = leader.x - position.x;
	const float ly = leader.y - position.y;
	const float leaderDistance = std::sqrt(lx * lx + ly * ly);
	if (leaderDistance > TeleportDistance) {
		position = Vector2{ leader.x - 20.0f, leader.y };
		return;
	}

	Vector2 move{ 0.0f, 0.0f };

	if (hasTarget) {
		const float dx = targetPosition.x - position.x;
		const float dy = targetPosition.y - position.y;
		const float d = std::sqrt(dx * dx + dy * dy);

		if (d > 0.001f) {
			const Vector2 toward{ dx / d, dy / d };
			constexpr float band = 8.0f;

			if (d > preferredDistance + band) {
				move = toward;                                          // too far: close in
			}
			else if (d < preferredDistance - band) {
				move = Vector2{ -toward.x * 0.9f, -toward.y * 0.9f };   // too close: back off
			}
			else {
				// In the sweet spot: circle the enemy, switching direction every 1.5 seconds.
				const float side = (static_cast<int>(strafeClock / 1.5f) % 2 == 0) ? 1.0f : -1.0f;
				move = Vector2{ -toward.y * side * 0.5f, toward.x * side * 0.5f };
			}
		}
	}
	else if (leaderDistance > FollowDistance) {
		move = Vector2{ lx / leaderDistance, ly / leaderDistance };     // no enemy: follow the leader
	}

	const float length = std::sqrt(move.x * move.x + move.y * move.y);
	if (length > 0.0f) walkClock += deltaTime * 11.0f;
	const float targetStride = length > 0.0f ? std::sin(walkClock) : 0.0f;
	stride += (targetStride - stride) * std::min(1.0f, deltaTime * 14.0f);

	position.x += move.x * speed * deltaTime;
	position.y += move.y * speed * deltaTime;
}

// ---------- Targeting and attacks ----------

void Ally::updateCombat(float deltaTime, const std::vector<Enemy*>& targets)
{
	if (isDead()) return;

	invulnerableTimer = std::max(0.0f, invulnerableTimer - deltaTime);
	if (temporary) lifetimeLeft -= deltaTime;
	character.update(deltaTime);

	// Nearest living enemy that is close to us and not too far from the leader.
	const Enemy* nearest = nullptr;
	float best = engageRange * engageRange;

	for (const Enemy* enemy : targets) {
		if (enemy->isDead()) continue;

		const Vector2 p = enemy->getHitPosition();

		const float lx = p.x - leader.x;
		const float ly = p.y - leader.y;
		if (lx * lx + ly * ly > LeashRange * LeashRange) continue;

		const float dx = p.x - position.x;
		const float dy = p.y - position.y;
		const float d = dx * dx + dy * dy;
		if (d < best) { best = d; nearest = enemy; }
	}

	hasTarget = (nearest != nullptr);
	bool shooting = false;

	if (nearest) {
		targetPosition = nearest->getHitPosition();

		const float dx = targetPosition.x - position.x;
		const float dy = targetPosition.y - position.y;
		const float d = std::sqrt(dx * dx + dy * dy);

		if (d > 1.0f) {
			aimDirection = Vector2{ dx / d, dy / d };
			shooting = d <= attackRange + nearest->getHitRadius();
		}
	}

	const AttackContext context{ position, aimDirection, character.getDamage(), targets, false };
	character.updateWeapons(deltaTime, context, false, shooting);
}

// ---------- Rendering ----------

void Ally::render(Engine::Graphics::Renderer& renderer) const
{
	// A clone blinks when it is about to disappear.
	if (temporary && lifetimeLeft < ExpireWarning && static_cast<int>(lifetimeLeft * 8.0f) % 2 == 0) return;

	const Avatar& avatar = character.getAvatar();
	const AvatarPose pose{ position, aimDirection, stride, character.getHandPose(position, aimDirection) };

	avatar.renderBody(renderer, pose);
	character.renderWeaponEffects(renderer);
	character.renderHeldWeapon(renderer, position, aimDirection);
	avatar.renderUpper(renderer, pose);

	// A shadow clone has a pulsing violet ring so it is easy to tell apart.
	if (temporary) {
		const float pulse = std::sin(static_cast<float>(GetTime()) * 6.0f);
		renderer.drawCircleOutline(position, BodyRadius + 5.0f + pulse, Color{ 190, 110, 255, 170 });
	}

	if (invulnerableTimer > 0.0f && static_cast<int>(invulnerableTimer * 30.0f) % 2 == 0) {
		renderer.drawCircle(position, BodyRadius + 2.0f, Color{ 255, 40, 40, 110 });
	}

	if (character.getHp() < character.getMaxHp()) {
		constexpr float barWidth = 28.0f;
		const float ratio = std::clamp(character.getHp() / character.getMaxHp(), 0.0f, 1.0f);
		const Rectangle back{ position.x - barWidth * 0.5f, position.y - BodyRadius - 12.0f, barWidth, 4.0f };
		renderer.drawRectangle(back, Color{ 20, 20, 20, 200 });
		renderer.drawRectangle(Rectangle{ back.x, back.y, back.width * ratio, back.height }, Color{ 80, 200, 80, 255 });
		renderer.drawRectangleOutline(back, 1.0f, BLACK);
	}
}