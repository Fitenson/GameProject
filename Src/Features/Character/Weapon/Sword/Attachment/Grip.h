#pragma once

#include "SwordPart.h"

class Grip : public SwordPart {
public:
	float length() const override;
	void render(Engine::Graphics::Renderer& renderer, const SwordMount& mount) const override;
};