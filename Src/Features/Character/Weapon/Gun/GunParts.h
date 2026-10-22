#pragma once

#include "GunPart.h"
#include "GunMount.h"

// The body of the gun. Every gun starts with one.
class Receiver : public GunPart {
public:
	float length() const override;
	void render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const override;
};

// The tube bullets travel through. A longer barrel is faster and reaches farther.
class Barrel : public GunPart {
public:
	explicit Barrel(float length = 18.0f);
	float length() const override;
	void modify(GunStats& stats) const override;
	void render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const override;
private:
	float barrelLength;
};

// Mounted on top of the gun. Adds range, no length.
class Scope : public GunPart {
public:
	explicit Scope(float extraRange = 250.0f);
	float length() const override;
	void modify(GunStats& stats) const override;
	void render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const override;
private:
	float extraRange;
};

// Muzzle device: hides the flash, slightly slows bullets.
class Suppressor : public GunPart {
public:
	float length() const override;
	void modify(GunStats& stats) const override;
	void render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const override;
};

// Muzzle device: cuts recoil.
class Compensator : public GunPart {
public:
	float length() const override;
	void modify(GunStats& stats) const override;
	void render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const override;
};


// Thick receiver of a heavy weapon.
class HeavyReceiver : public GunPart {
public:
	float length() const override;
	void render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const override;
};

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