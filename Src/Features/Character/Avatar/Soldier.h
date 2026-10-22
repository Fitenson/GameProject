#pragma once

#include "Avatar.h"

class Soldier : public Avatar {
public:
	Soldier();

	void specialAbility() const override;
private:
};