#pragma once

#include <array>
#include "Engine/Core.h"
#include "../Weapon.h"
#include "SwordPart.h"
#include "SwordStats.h"

struct Hand {
	float side;                      // +1 = right hand, -1 = left hand
	float swingTime = 0.0f;
	float centerAngle = 0.0f;        // aim angle when this hand's swing started
	Vector2 origin{ 0.0f, 0.0f };    // hand position in the world
};

class Sword : public Weapon {
public:
	Sword();
	~Sword() = default;

	void addPart(UniquePtr<SwordPart> part);

	void renderHeld(Engine::Graphics::Renderer& renderer, const Vector2& playerPosition, const Vector2& direction) const override;
	void renderEffects(Engine::Graphics::Renderer& renderer) const override;

protected:
	void onAttack(const AttackContext& context) override;
	void onUpdate(float deltaTime, const AttackContext& context) override;

private:
	static constexpr float HolderGap = 8.0f;     // distance from the hand to the first part
	static constexpr float HandOffset = 7.0f;    // how far each hand sits from the body's center line
	static constexpr float RestSpread = 0.45f;   // radians the idle sword angles outward

	UniquePtr<SwordPart> parts;   // pommel -> grip -> guard -> blade
	SwordPart* tail;
	SwordStats stats;
	float tipOffset = HolderGap;              // also the hit range

	float swingTime = 0.0f;
	float centerAngle = 0.0f;
	float swingSign = 1.0f;                   // alternates, so swings go back and forth
	Vector2 holder{ 0.0f, 0.0f };
	std::array<Hand, 2> hands{ Hand{ -1.0f }, Hand{ 1.0f } };
	size_t nextHand = 1;                      // the right hand swings first

	void rebuild();
	static Vector2 playerHand(const Vector2& playerPosition, const Vector2& direction, float side);
	float swingAngle(const Hand& hand) const;                                 // while swinging
	float handAngle(const Hand& hand, const Vector2& direction) const;       // swinging or resting
};