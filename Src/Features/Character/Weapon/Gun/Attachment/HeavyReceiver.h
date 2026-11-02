#pragma once

#include "GunPart.h"

class HeavyReceiver : public GunPart {
public:
	float length() const override;
	void render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const override;
};