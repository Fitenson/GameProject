#pragma once

#include "SwordPart.h"

class Fuller : public SwordPart {
public:
	float length() const override;
	void modify(SwordStats& stats) const override;
	void render(Engine::Graphics::Renderer& renderer, const SwordMount& mount) const override;
};