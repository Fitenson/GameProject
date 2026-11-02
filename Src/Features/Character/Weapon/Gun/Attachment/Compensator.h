#pragma once

#include "GunPart.h"

class Compensator : public GunPart {
public:
	float length() const override;
	void modify(GunStats& stats) const override;
	void render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const override;
};