#pragma once

#include "SwordPart.h"

class Pommel : public SwordPart {
public:
	float length() const override;
	void render(Engine::Graphics::Renderer& renderer, const SwordMount& mount) const override;
};

class Grip : public SwordPart {
public:
	float length() const override;
	void render(Engine::Graphics::Renderer& renderer, const SwordMount& mount) const override;
};

// Crossguard: a short bar across the blade axis.
class Guard : public SwordPart {
public:
	float length() const override;
	void render(Engine::Graphics::Renderer& renderer, const SwordMount& mount) const override;
};

// A longer blade is heavier, so the swing is slower. A sharper one hits harder.
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