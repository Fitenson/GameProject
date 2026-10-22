#pragma once

#include "Engine/Graphics.h"

class NavigationStack;

class Screen {
public:
	virtual ~Screen() = default;

	virtual void onEnter() {}    // pushed onto the stack
	virtual void onExit() {}     // popped or replaced
	virtual void onPause() {}    // another screen was pushed on top
	virtual void onResume() {}   // the screen above was popped

	// Only the top screen is updated. Use `navigation` to push, pop or replace.
	virtual void update(float deltaTime, NavigationStack& navigation) = 0;
	virtual void render(Engine::Graphics::Renderer& renderer) = 0;

	// A screen that is not opaque lets the screen below it draw first (overlays).
	virtual bool isOpaque() const { return true; }
};