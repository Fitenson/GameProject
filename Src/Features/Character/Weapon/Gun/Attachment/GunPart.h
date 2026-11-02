#pragma once

#include "Engine/Core.h"
#include "Engine/Graphics.h"
#include "../GunStats.h"
#include "../GunMount.h"

class GunPart {
public:
	GunPart() = default;
	virtual ~GunPart() = default;

	// How far this part extends the gun along the barrel axis.
	// Mounted accessories (a scope) return 0.
	virtual float length() const { return 0.0f; }

	// Change the gun's stats. Default: no effect.
	virtual void modify(GunStats& stats) const {}

	virtual void render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const {}

	static void drawSegment(Engine::Graphics::Renderer& renderer, const GunMount& mount,
		float from, float to, float thickness, Color color, float sideways = 0.0f)
	{
		renderer.drawLine(mount.pointAt(from, sideways), mount.pointAt(to, sideways), thickness, color);
	}
};