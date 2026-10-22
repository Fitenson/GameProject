#include <cmath>
#include <algorithm>
#include "Zombie.h"

Zombie::Zombie(const Vector2& position) : Entity(position), maxHp(100.0f), hp(100.0f), damage(20.0f), speed(30.0f), target(position) {}

float Zombie::getMaxHp() const { return maxHp; }
float Zombie::getHp() const { return hp; }
void Zombie::setHp(float newHp) {
	hp = newHp;
	trailHp = std::max(trailHp, hp);
}
float Zombie::getDamage() const { return damage; }
float Zombie::getSpeed() const { return speed; }

void Zombie::setTarget(const Vector2& newTarget) { target = newTarget; }

void Zombie::update(float deltaTime)
{
	healthBarTimer = std::max(0.0f, healthBarTimer - deltaTime);
	hitFlash = std::max(0.0f, hitFlash - deltaTime);

	if (isDead()) {
		deathTimer = std::min(DeathDuration, deathTimer + deltaTime);
		return;
	}

	if (trailDelay > 0.0f) {
		trailDelay -= deltaTime;
	}
	else if (trailHp > hp) {
		trailHp = std::max(hp, trailHp - maxHp * 0.6f * deltaTime);   // drains 60% of max hp per second
	}

	if (stunTimer > 0.0f) {
		stunTimer -= deltaTime;
		return;
	}

	const float directionX = target.x - position.x;
	const float directionY = target.y - position.y;
	const float distance = std::sqrt(directionX * directionX + directionY * directionY);

	if (distance < 1.0f) return;

	position.x += directionX / distance * speed * deltaTime;
	position.y += directionY / distance * speed * deltaTime;
}

void Zombie::render(Engine::Graphics::Renderer& renderer) const
{
	const float tint = std::clamp(static_cast<float>(level - 1) / 9.0f, 0.0f, 1.0f);
	const auto mix = [tint](int calm, int fierce) { return static_cast<unsigned char>(calm + (fierce - calm) * tint); };
	const Color levelColor{ mix(90, 165), mix(140, 55), mix(60, 50), 255 };

	Color body = stunTimer > 0.0f ? Color{ 200, 200, 90, 255 } : levelColor;
	if (hitFlash > 0.0f) body = WHITE;

	renderer.drawCircle(position, Radius, body);
	renderer.drawCircleOutline(position, Radius, BLACK);

	if (healthBarTimer <= 0.0f) return;

	const float fade = std::min(1.0f, healthBarTimer / HealthBarFade);
	const auto alpha = [fade](unsigned char a) { return static_cast<unsigned char>(a * fade); };

	constexpr float barWidth = 34.0f;
	constexpr float barHeight = 5.0f;

	const Rectangle back{ position.x - barWidth * 0.5f, position.y - Radius - 12.0f, barWidth, barHeight };

	const float ratio = std::clamp(hp / maxHp, 0.0f, 1.0f);
	const float trailRatio = std::clamp(trailHp / maxHp, 0.0f, 1.0f);

	Color fill = ratio > 0.5f ? Color{ 80, 200, 80, 255 }
		: ratio > 0.25f ? Color{ 230, 200, 50, 255 }
	: Color{ 220, 60, 50, 255 };

	renderer.drawRectangle(back, Color{ 20, 20, 20, alpha(200) });
	renderer.drawRectangle(Rectangle{ back.x, back.y, back.width * trailRatio, back.height }, Color{ 235, 235, 235, alpha(200) });
	renderer.drawRectangle(Rectangle{ back.x, back.y, back.width * ratio, back.height }, Color{ fill.r, fill.g, fill.b, alpha(255) });
	renderer.drawRectangleOutline(back, 1.0f, Color{ 0, 0, 0, alpha(255) });
	renderer.drawText("Lv " + std::to_string(level), Vector2{ back.x, back.y - 11.0f }, 10, Color{ 255, 255, 255, alpha(255) });
}

Vector2 Zombie::getHitPosition() const { return position; }

float Zombie::getHitRadius() const { return Radius; }

void Zombie::takeDamage(float amount) { 
	if (isDead()) return; //	Dead enemy cannot be hit

	hp = std::max(0.0f, hp - amount);
	hitFlash = HitFlashDuration;

	if (hp <= 0.0f) {
		// The killing blow starts the death animation and hides the bar.
		deathTimer = 0.0f;
		stunTimer = 0.0f;
		healthBarTimer = 0.0f;
		return;
	}

	healthBarTimer = HealthBarDisplayDuration;   // every hit restarts the 5 seconds
	trailDelay = TrailDelay;
}

void Zombie::stun(float seconds) { stunTimer = std::max(stunTimer, seconds); }

bool Zombie::isDead() const { return hp <= 0.0f; }
bool Zombie::isRemovable() const { return isDead() && deathTimer >= DeathDuration; }

void Zombie::renderDeath(Engine::Graphics::Renderer& renderer) const
{
	const float t = std::clamp(deathTimer / DeathDuration, 0.0f, 1.0f);   // 0 -> 1
	const float ease = 1.0f - (1.0f - t) * (1.0f - t);                    // fast start, slow end
	const unsigned char alpha = static_cast<unsigned char>(255.0f * (1.0f - t));

	// Body: flashes white at first, then shrinks and fades.
	const Color base = hitFlash > 0.0f ? WHITE : Color{ 90, 140, 60, 255 };
	const float bodyRadius = Radius * (1.0f - 0.5f * ease);
	renderer.drawCircle(position, bodyRadius, Color{ base.r, base.g, base.b, alpha });

	// Shockwave ring.
	renderer.drawCircleOutline(position, Radius * (1.0f + 1.8f * ease), Color{ 200, 230, 160, alpha });

	// Blood puffs flying outward in 8 directions.
	constexpr int puffs = 8;
	const float puffSize = Radius * 0.28f * (1.0f - t);
	for (int i = 0; i < puffs; ++i) {
		const float angle = (6.2831853f / puffs) * i + 0.4f;
		const float distance = Radius * (0.5f + 2.0f * ease);
		const Vector2 p{ position.x + std::cos(angle) * distance, position.y + std::sin(angle) * distance };
		renderer.drawCircle(p, puffSize, Color{ 70, 110, 40, alpha });
	}
}

float Zombie::updateAttack(float deltaTime, const Vector2& targetPosition, float targetRadius)
{
	attackCooldownLeft = std::max(0.0f, attackCooldownLeft - deltaTime);

	// Dead or stunned zombies can't attack, and a stun cancels a wind-up.
	if (isDead() || stunTimer > 0.0f) {
		attacking = false;
		return 0.0f;
	}

	const float dx = targetPosition.x - position.x;
	const float dy = targetPosition.y - position.y;
	const float distance = std::sqrt(dx * dx + dy * dy);
	const float reach = Radius + targetRadius + AttackReach;

	if (attacking) {
		windupLeft -= deltaTime;
		if (windupLeft <= 0.0f) {
			attacking = false;
			attackCooldownLeft = AttackCooldown;
			if (distance <= reach) return damage;   // the target dodged if it moved away
		}
		return 0.0f;
	}

	if (attackCooldownLeft <= 0.0f && distance <= reach) {
		attacking = true;
		windupLeft = AttackWindup;
	}
	return 0.0f;
}

void Zombie::applyLevel(int newLevel, float healthScale, float speedScale, float damageScale)
{
	level = newLevel;
	maxHp = BaseHp * healthScale;
	hp = maxHp;
	trailHp = maxHp;
	damage = BaseDamage * damageScale;
	speed = BaseSpeed * speedScale;
}

int Zombie::getLevel() const { return level; }