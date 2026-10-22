#pragma once

#include "Avatar.h"

class Tanker : public Avatar {
public:
	Tanker();

	void specialAbility() const;

	float getShieldRatio() const override;
};