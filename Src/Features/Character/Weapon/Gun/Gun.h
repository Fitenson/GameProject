#pragma once

#include <vector>
#include "Engine/Core.h"
#include "Engine/Utils.h"
#include "../Weapon.h"
#include "Bullet.h"
#include "GunParts.h"
#include "GunStats.h"

class Gun : public Weapon
{
public:
	Gun();
	Gun(const std::string& name, float cooldown, float damageMultiplier);

	// Attach a part at the end of the chain (closest to the tip last).
	void addPart(UniquePtr<GunPart> part);

	Iterator<GunPart> begin() const;
	Iterator<GunPart> end() const;

	void renderHeld(Engine::Graphics::Renderer& renderer, const Vector2& playerPosition, const Vector2& direction) const override;
	void renderEffects(Engine::Graphics::Renderer& renderer) const override;

protected:
	void onAttack(const AttackContext& context) override;
	void onUpdate(float deltaTime, const AttackContext& context) override;

private:
	static constexpr float HolderGap = 8.0f;

	UniquePtr<GunPart> parts;
	GunPart* tail;
	GunStats stats;
	float tipOffset = HolderGap;

	std::vector<Bullet> bullets;
	float flashTimer = 0.0f;
	float recoil = 0.0f;

	void rebuild();
};