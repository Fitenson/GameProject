#pragma once

#include <raylib.h>
#include "Engine/DataStructure.h"
#include "../Game/Entity.h"
#include "../Character/Character.h"
#include "../Monster/Enemy.h"

class Player : public Entity, public Engine::DataStructure::Element {
public:
	Player(Character character, const Vector2& position);

	Character& getCharacter();
	const Character& getCharacter() const;
	
	void setAimTarget(const Vector2& worldTarget);
	Vector2 getAimDirection() const;

	int getCoins();
	void setCoins(int coins);

	Vector2 getHitPosition() const;
	float getHitRadius() const;
	void takeDamage(float amount);
	bool isDead() const;

	void update(float deltaTime) override;
	void render(Engine::Graphics::Renderer& renderer) const override;
	void updateWeapons(float deltaTime, const std::vector<Enemy*>& targets);

private:
	Character character;
	Vector2 aimTarget{ 0.0f, 0.0f };
	Vector2 aimDirection{ 1.0f, 0.0f };
	float invulnerableTimer = 0.0f;
	float walkClock = 0.0f;
	float stride = 0.0f;
	int coins = 0;

	static constexpr float HitRadius = 12.0f;
	static constexpr float InvulnerableTime = 0.5f;	// Delay period after a hit
	static constexpr float ShieldRadius = 28.0f;
};