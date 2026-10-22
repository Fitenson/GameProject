#pragma once

#include <raylib.h>
#include "Engine/Graphics.h"
#include "../Game/Entity.h"
#include "Enemy.h"

class Zombie : public Entity, public Enemy {
public:
	Zombie(const Vector2& position = Vector2{ 0.0f, 0.0f });

	static constexpr float Radius = 12.0f;
	static constexpr float BaseHp = 100.0f;
	static constexpr float BaseDamage = 10.0f;
	static constexpr float BaseSpeed = 80.0f;

	float getMaxHp() const;
	float getHp() const;
	void setHp(float hp);

	float getDamage() const;
	float getSpeed() const;

	void applyLevel(int level, float healthScale, float speedScale, float damageScale);
	int getLevel() const;

	void setTarget(const Vector2& target);

	void update(float deltaTime) override;
	float updateAttack(float deltaTime, const Vector2& targetPosition, float targetRadius) override;
	void render(Engine::Graphics::Renderer& renderer) const override;
	void renderDeath(Engine::Graphics::Renderer& renderer) const override;

	Vector2 getHitPosition() const override;
	float getHitRadius() const override;
	void takeDamage(float damage) override;
	void stun(float seconds) override;
	bool isDead() const override;
	bool isRemovable() const override;

private:
	float maxHp;
	float hp;
	float damage;
	float speed;
	float stunTimer;
	Vector2 target;
	int level = 1;
};