#pragma once

#include "SwordPart.h"

class Blade : public SwordPart {
public:
	explicit Blade(float length = 52.0f, float sharpness = 1.0f);
	float length() const override;
	void modify(SwordStats& stats) const override;
	void render(Engine::Graphics::Renderer& renderer, const SwordMount& mount) const override;

private:
	float bladeLength;
	float sharpness;
};