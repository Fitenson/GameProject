#pragma once

#include "GunPart.h"

// Long, thick barrel with cooling fins. Hard to handle, but it hits with a big flash.
class HeavyBarrel : public GunPart {
public:
	explicit HeavyBarrel(float length = 30.0f);
	float length() const override;
	void modify(GunStats& stats) const override;
	void render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const override;
private:
	float barrelLength;
};