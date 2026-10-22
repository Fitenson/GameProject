#include <utility>
#include "NavigationStack.h"

void NavigationStack::push(UniquePtr<Screen> screen)
{
	if (screen) enqueue(Action::Push, std::move(screen));
}
void NavigationStack::pop() { enqueue(Action::Pop); }
void NavigationStack::replace(UniquePtr<Screen> screen)
{
	if (screen) enqueue(Action::Replace, std::move(screen));
}
void NavigationStack::clear() { enqueue(Action::Clear); }

Screen* NavigationStack::getCurrent() { return screens.empty() ? nullptr : screens.back().get(); }
const Screen* NavigationStack::getCurrent() const { return screens.empty() ? nullptr : screens.back().get(); }
bool NavigationStack::isEmpty() const { return screens.empty(); }
size_t NavigationStack::size() const { return screens.size(); }

void NavigationStack::enqueue(Action action, UniquePtr<Screen> screen)
{
	pending.push_back(Command{ action, std::move(screen) });
	flush();
}

// Applies queued requests in order. Does nothing while a screen is mid-update.
void NavigationStack::flush()
{
	if (busy) return;
	busy = true;

	while (!pending.empty()) {
		Command command = std::move(pending.front());
		pending.erase(pending.begin());
		apply(command);
	}

	busy = false;
}

void NavigationStack::apply(Command& command)
{
	switch (command.action) {
	case Action::Push:
		doPush(std::move(command.screen), true);
		break;
	case Action::Pop:
		doPop(true);
		break;
	case Action::Replace:
		doPop(false);
		doPush(std::move(command.screen), false);
		break;
	case Action::Clear:
		while (!screens.empty()) doPop(false);
		break;
	}
}

void NavigationStack::doPush(UniquePtr<Screen> screen, bool pausePrevious)
{
	if (pausePrevious && !screens.empty()) screens.back()->onPause();
	screens.push_back(std::move(screen));
	screens.back()->onEnter();
}

void NavigationStack::doPop(bool resumePrevious)
{
	if (screens.empty()) return;

	screens.back()->onExit();
	screens.pop_back();   // the screen is destroyed here

	if (resumePrevious && !screens.empty()) screens.back()->onResume();
}

void NavigationStack::update(float deltaTime)
{
	if (screens.empty()) return;

	busy = true;                                   // defer push/pop requests made by the screen
	screens.back()->update(deltaTime, *this);
	busy = false;

	flush();
}

void NavigationStack::render(Engine::Graphics::Renderer& renderer)
{
	if (screens.empty()) return;

	// Find the lowest screen that has to be drawn: the top one, or the first opaque one below it.
	size_t first = screens.size() - 1;
	while (first > 0 && !screens[first]->isOpaque()) --first;

	for (size_t i = first; i < screens.size(); ++i) {
		screens[i]->render(renderer);
	}
}