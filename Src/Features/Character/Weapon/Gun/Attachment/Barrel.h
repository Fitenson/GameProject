#pragma once

#include "GunPart.h"

class Barrel : public GunPart {
public:
	Barrel(float length = 18.0f);

	float length() const override;
	void modify(GunStats& stats) const override;
	void render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const override;

private:
	float barrelLength;
};