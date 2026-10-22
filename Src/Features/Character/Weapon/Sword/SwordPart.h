#pragma once

#include <cmath>
#include <raylib.h>
#include "Engine/Core.h"
#include "Engine/Graphics.h"
#include "SwordStats.h"
#include "SwordMount.h"

class SwordPart {
public:
	virtual ~SwordPart() = default;

	SwordPart* getNext() { return next.get(); }
	const SwordPart* getNext() const { return next.get(); }
	void setNext(UniquePtr<SwordPart> part) { this->next = std::move(part); }

	virtual float length() const = 0;                       // extension along the blade axis
	virtual void modify(SwordStats& stats) const {}         // default: no effect
	virtual void render(Engine::Graphics::Renderer& renderer, const SwordMount& mount) const = 0;

protected:
	static void segment(Engine::Graphics::Renderer& renderer, const SwordMount& mount, float from, float to, float thickness, Color color)
	{
		renderer.drawLine(mount.pointAt(from, 0.0f), mount.pointAt(to, 0.0f), thickness, color);
	}

private:
	UniquePtr<SwordPart> next;
};