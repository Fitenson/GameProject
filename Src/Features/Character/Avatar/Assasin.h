#pragma once

#include <vector>
#include "Engine/Core.h"
#include "../NPC/Ally.h"
#include "Avatar.h"

class Assasin : public Avatar {
private:
	static constexpr float AbilityDuration = 10.0f;

public:
	Assasin();

	AvatarLook getLook() const override;

	std::string getAbilityName() const override { return "Shadow Clone"; }
	float getAbilityDuration() const override { return 10.0f; }
	float getAbilityCooldown() const override { return 25.0f; }

	// The avatar only says it wants a clone. GameState creates the Ally,
	// so Avatar never depends on Ally (no circular include).
	bool summonsClone() const override { return true; }
};