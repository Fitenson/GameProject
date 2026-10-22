#include <cmath>
#include <utility>
#include <algorithm>
#include "Player.h"

Player::Player(Character character, const Vector2& position)
	: Entity(position), character(std::move(character)), aimTarget(position) {}

Character& Player::getCharacter() { return character; }
const Character& Player::getCharacter() const { return character; }

void Player::setAimTarget(const Vector2& worldTarget) { aimTarget = worldTarget; }
Vector2 Player::getAimDirection() const { return aimDirection; }

Vector2 Player::getHitPosition() const { return position; }
float Player::getHitRadius() const { return HitRadius; }
bool Player::isDead() const { return character.isDead(); }

void Player::takeDamage(float amount)
{
	if (isDead() || invulnerableTimer > 0.0f) return;   // grace period stops damage stacking
	character.takeDamage(amount);
	invulnerableTimer = InvulnerableTime;
}

void Player::render(Engine::Graphics::Renderer& renderer) const
{
	constexpr float size = 24.0f;
	const Rectangle body{ position.x - size * 0.5f, position.y - size * 0.5f, size, size };

	// Blink red while invulnerable.
	const bool blink = invulnerableTimer > 0.0f && (static_cast<int>(invulnerableTimer * 20.0f) % 2 == 0);
	renderer.drawRectangle(body, blink ? RED : BLUE);
	renderer.drawRectangleOutline(body, 2.0f, WHITE);

	character.renderWeaponEffects(renderer);
	character.renderHeldWeapon(renderer, position, aimDirection);

	const Shield* shield = character.getShield();
	if (shield) shield->render(renderer, position, ShieldRadius);

	// Bars sit above the shield bubble (or above the body when there is none).
	constexpr float barWidth = 36.0f;
	constexpr float barHeight = 5.0f;
	const float top = position.y - (shield ? ShieldRadius : size * 0.5f) - 10.0f;

	if (character.getHp() < character.getMaxHp()) {
		const float ratio = std::clamp(character.getHp() / character.getMaxHp(), 0.0f, 1.0f);
		const Rectangle back{ position.x - barWidth * 0.5f, top, barWidth, barHeight };

		renderer.drawRectangle(back, Color{ 20, 20, 20, 200 });
		renderer.drawRectangle(Rectangle{ back.x, back.y, back.width * ratio, back.height }, Color{ 80, 200, 80, 255 });
		renderer.drawRectangleOutline(back, 1.0f, BLACK);
	}

	if (shield) {
		shield->renderBar(renderer, Rectangle{ position.x - barWidth * 0.5f, top - barHeight - 3.0f, barWidth, barHeight });
	}
}

void Player::update(float deltaTime)
{
	invulnerableTimer = std::max(0.0f, invulnerableTimer - deltaTime);
	character.update(deltaTime);	//	Shield timers and repair

	const float speedScale = 4.0f;
	const float speed = character.getMovementSpeed() * speedScale * character.getMovementMultiplier();

	Vector2 move{ 0.0f, 0.0f };
	if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    move.y -= 1.0f;
	if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  move.y += 1.0f;
	if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  move.x -= 1.0f;
	if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) move.x += 1.0f;

	const float length = std::sqrt(move.x * move.x + move.y * move.y);
	if (length > 0.0f) {
		move.x /= length;
		move.y /= length;
	}

	position.x += move.x * speed * deltaTime;
	position.y += move.y * speed * deltaTime;

	//	Aim at the mouse
	const float aimX = aimTarget.x - position.x;
	const float aimY = aimTarget.y - position.y;
	const float aimLength = std::sqrt(aimX * aimX + aimY * aimY);

	if (aimLength > 1.0f) {
		aimDirection = Vector2{ aimX / aimLength, aimY / aimLength };
	}
}

void Player::updateWeapons(float deltaTime, const std::vector<Enemy*>& targets)
{
	// Input: number keys 1-9 or the mouse wheel switch weapons.
	for (int i = 0; i < 9; ++i) {
		if (IsKeyPressed(KEY_ONE + i)) character.selectWeapon(static_cast<size_t>(i));
	}
	const float wheel = GetMouseWheelMove();
	if (wheel > 0.0f) character.selectNextWeapon();
	if (wheel < 0.0f) character.selectPreviousWeapon();

	const AttackContext context{ position, aimDirection, character.getDamage(), targets };
	character.updateWeapons(
		deltaTime,
		context,
		IsMouseButtonPressed(MOUSE_BUTTON_LEFT),
		IsMouseButtonDown(MOUSE_BUTTON_LEFT)
	);
}