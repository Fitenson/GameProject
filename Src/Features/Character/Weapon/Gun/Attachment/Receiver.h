#pragma once

#include "GunPart.h"

// The body of the gun. Every gun starts with one.
class Receiver : public GunPart {
public:
	float length() const override;
	void render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const override;
};
