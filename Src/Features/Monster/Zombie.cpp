#include <cmath>
#include <algorithm>
#include "Zombie.h"

Zombie::Zombie(const Vector2& position) : 
Entity(position), 
maxHp(100.0f), 
hp(100.0f), 
damage(20.0f), 
speed(30.0f), 
target(position),
facingDirection(1.0f, 0.0f),
walkClock(0.0f) {}

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
		trailHp = std::max(hp, trailHp - maxHp * 0.6f * deltaTime);
	}

	if (stunTimer > 0.0f) {
		stunTimer -= deltaTime;
		return;
	}

	const float directionX = target.x - position.x;
	const float directionY = target.y - position.y;
	const float distance = std::sqrt(directionX * directionX + directionY * directionY);

	if (distance < 1.0f) return;

	facingDirection.x = directionX / distance;
	facingDirection.y = directionY / distance;
	walkClock += deltaTime * (4.0f * speed * 0.08f);

	position.x += directionX / distance * speed * deltaTime;
	position.y += directionY / distance * speed * deltaTime;
}

void Zombie::render(Engine::Graphics::Renderer& renderer) const
{
	const float tint = std::clamp(static_cast<float>(level - 1) / 9.0f, 0.0f, 1.0f);
	const auto mix = [tint](int calm, int fierce) { return static_cast<unsigned char>(calm + (fierce - calm) * tint); };

	Color skin{ mix(126, 170), mix(156, 96), mix(104, 84), 255 };
	Color cloth{ mix(70, 92), mix(74, 52), mix(92, 56), 255 };
	const Color clothDark{ static_cast<unsigned char>(cloth.r * 0.6f), static_cast<unsigned char>(cloth.g * 0.6f),
						   static_cast<unsigned char>(cloth.b * 0.6f), 255 };
	const Color outline{ 24, 20, 20, 255 };

	if (stunTimer > 0.0f) { skin = Color{ 205, 205, 110, 255 }; }
	if (hitFlash > 0.0f) { skin = WHITE; cloth = Color{ 235, 235, 235, 255 }; }

	// Local axes: forward = toward the target, side = the zombie's right-hand side.
	const Vector2 f = facingDirection;
	const Vector2 s{ -f.y, f.x };
	const auto at = [&](float a, float b) {
		return Vector2{ position.x + f.x * a + s.x * b, position.y + f.y * a + s.y * b };
		};

	const float step = std::sin(walkClock);
	const float lunge = attacking ? std::clamp(1.0f - windupLeft / AttackWindup, 0.0f, 1.0f) : 0.0f;

	// Shadow
	renderer.drawCircle(Vector2{ position.x + 1.5f, position.y + 2.5f }, Radius + 1.0f, Color{ 0, 0, 0, 70 });

	// Legs: dragging feet that swing forward and back.
	for (float side : { -1.0f, 1.0f }) {
		const Vector2 hip = at(-1.0f, side * 3.0f);
		const Vector2 foot = at(1.0f + side * step * 3.5f, side * 3.2f);
		renderer.drawLine(hip, foot, 5.0f, outline);
		renderer.drawLine(hip, foot, 3.6f, Color{ 52, 54, 66, 255 });
		renderer.drawCircle(foot, 2.6f, Color{ 38, 32, 30, 255 });
	}

	// Torn torso, leaning into the lunge.
	const float torso = 0.0f + lunge * 2.0f;
	renderer.drawLine(at(torso, -6.0f), at(torso, 6.0f), 11.0f, outline);
	renderer.drawCircle(at(torso, -6.0f), 5.5f, outline);
	renderer.drawCircle(at(torso, 6.0f), 5.5f, outline);
	renderer.drawLine(at(torso, -6.0f), at(torso, 6.0f), 9.0f, cloth);
	renderer.drawCircle(at(torso, -6.0f), 4.5f, cloth);
	renderer.drawCircle(at(torso, 6.0f), 4.5f, cloth);

	// Rips, a bloodstain and an exposed patch of skin.
	renderer.drawLine(at(torso - 2.0f, -4.0f), at(torso + 2.5f, -2.0f), 1.5f, clothDark);
	renderer.drawLine(at(torso - 1.0f, 3.0f), at(torso + 3.0f, 4.5f), 1.5f, clothDark);
	renderer.drawCircle(at(torso + 0.5f, 2.5f), 2.4f, Color{ 120, 26, 28, 210 });
	renderer.drawCircle(at(torso - 2.5f, -2.0f), 1.8f, skin);

	// Arms stretched out in front. They reach further during the attack.
	const float reach = 11.5f + lunge * 5.5f;
	for (float side : { -1.0f, 1.0f }) {
		const float sway = std::sin(walkClock * 0.5f + (side > 0.0f ? 0.0f : 1.7f)) * 1.8f * (1.0f - lunge);
		const Vector2 shoulder = at(torso, side * 6.5f);
		const Vector2 elbow = at(torso + reach * 0.5f, side * (7.5f + sway));
		const Vector2 hand = at(torso + reach, side * (3.5f + sway));

		renderer.drawLine(shoulder, elbow, 5.5f, outline);
		renderer.drawLine(elbow, hand, 5.0f, outline);
		renderer.drawLine(shoulder, elbow, 4.0f, cloth);      // torn sleeve
		renderer.drawLine(elbow, hand, 3.4f, skin);           // bare forearm
		renderer.drawCircle(hand, 3.2f, outline);
		renderer.drawCircle(hand, 2.4f, skin);
		renderer.drawCircle(at(torso + reach + 1.5f, side * (3.5f + sway)), 1.2f, Color{ 150, 30, 30, 255 });   // bloody fingertips
	}

	// Head: it lolls from side to side, and sits further forward in a lunge.
	const Vector2 head = at(1.5f + lunge * 2.5f, std::sin(walkClock * 0.5f) * 1.4f);
	renderer.drawCircle(head, 6.2f, outline);
	renderer.drawCircle(head, 5.2f, skin);

	// Matted hair on the back of the head, a wound on the side, a gaping mouth at the front.
	const Vector2 hair = Vector2{ head.x - f.x * 2.0f, head.y - f.y * 2.0f };
	renderer.drawCircle(hair, 3.6f, Color{ 40, 34, 30, 255 });
	renderer.drawCircle(Vector2{ hair.x + s.x * 2.5f, hair.y + s.y * 2.5f }, 2.0f, Color{ 52, 44, 38, 255 });
	renderer.drawCircle(Vector2{ head.x + s.x * 3.2f - f.x * 0.5f, head.y + s.y * 3.2f - f.y * 0.5f }, 1.7f, Color{ 150, 28, 30, 255 });
	renderer.drawLine(Vector2{ head.x + f.x * 4.2f - s.x * 1.4f, head.y + f.y * 4.2f - s.y * 1.4f },
		Vector2{ head.x + f.x * 4.2f + s.x * 1.4f, head.y + f.y * 4.2f + s.y * 1.4f }, 1.6f, Color{ 60, 10, 12, 255 });

	// Glowing eyes. They flare red during an attack.
	const Color eye = attacking ? Color{ 255, 60, 40, 255 } : Color{ 255, 215, 90, 255 };
	for (float side : { -1.0f, 1.0f }) {
		const Vector2 e = Vector2{ head.x + f.x * 2.8f + s.x * side * 2.0f, head.y + f.y * 2.8f + s.y * side * 2.0f };
		renderer.drawCircle(e, 1.5f, Color{ eye.r, eye.g, eye.b, 90 });
		renderer.drawCircle(e, 0.9f, eye);
	}

	// ---- Health bar (unchanged) ----
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