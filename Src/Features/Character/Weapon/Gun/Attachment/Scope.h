#pragma once

#include "GunPart.h"

class Scope : public GunPart {
public:
	Scope(float extraRange = 250.0f);

	float length() const override;
	void modify(GunStats& stats) const override;
	void render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const override;
private:
	float extraRange;
};