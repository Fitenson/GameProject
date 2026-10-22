#pragma once

#include "Engine/Core.h"
#include "Engine/Graphics.h"
#include "GunStats.h"
#include "GunMount.h"

class GunPart {
public:
	virtual ~GunPart() = default;

	GunPart* getNext() { return next.get(); }

	const GunPart* getNext() const { return next.get(); }

	virtual void setNext(UniquePtr<GunPart> next) { this->next = std::move(next); }

	// How far this part extends the gun along the barrel axis.
	// Mounted accessories (a scope) return 0.
	virtual float length() const = 0;

	// Change the gun's stats. Default: no effect.
	virtual void modify(GunStats& stats) const {}

	virtual void render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const = 0;

	static void drawSegment(Engine::Graphics::Renderer& renderer, const GunMount& mount,
		float from, float to, float thickness, Color color, float sideways = 0.0f)
	{
		renderer.drawLine(mount.pointAt(from, sideways), mount.pointAt(to, sideways), thickness, color);
	}

private:
	UniquePtr<GunPart> next;
};