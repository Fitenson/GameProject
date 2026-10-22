#pragma once

#include <vector>
#include "../Core.h"
#include "Engine/Graphics.h"
#include "Screen.h"

class NavigationStack {
public:
	NavigationStack() = default;
	~NavigationStack() { clear(); }

	NavigationStack(const NavigationStack&) = delete;
	NavigationStack& operator=(const NavigationStack&) = delete;

	// Requests are applied immediately, or right after the running update() if called from a screen.
	void push(UniquePtr<Screen> screen);
	void pop();
	void replace(UniquePtr<Screen> screen);   // pop + push, without resuming the screen below
	void clear();                             // pops every screen, top first

	Screen* getCurrent();
	const Screen* getCurrent() const;
	bool isEmpty() const;
	size_t size() const;

	void update(float deltaTime);
	void render(Engine::Graphics::Renderer& renderer);

private:
	enum class Action { Push, Pop, Replace, Clear };

	struct Command {
		Action action;
		UniquePtr<Screen> screen;
	};

	std::vector<UniquePtr<Screen>> screens;   // back() is the top
	std::vector<Command> pending;
	bool busy = false;                        // true while updating or applying

	void enqueue(Action action, UniquePtr<Screen> screen = nullptr);
	void flush();
	void apply(Command& command);
	void doPush(UniquePtr<Screen> screen, bool pausePrevious);
	void doPop(bool resumePrevious);
};