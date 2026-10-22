#include <vector>
#include <utility>
#include <raylib.h>
#include "Engine/UI.h"
#include "MainMenu.h"
#include "AvatarMenu.h"

namespace UI = Engine::UI;
namespace Layout = Engine::Layout;

namespace {
	constexpr Color DarkGreen{ 20, 160, 133, 255 };

	UI::Modifier buttonStyle()
	{
		return UI::Modifier()
			.background(WHITE)
			.hover(Color{ 225, 250, 244, 255 })
			.textColor(DarkGreen)
			.fontSize(30)
			.borderThickness(0.0f)
			.borderColor(BLANK);
	}
}

MainMenu::MainMenu(std::function<void(AvatarVariant)> onStart, std::function<void()> onQuit)
	: title("CAPTURE THE FLAG", UI::Modifier().fontSize(50).textColor(WHITE)),
	playButton(Rectangle{}, "PLAY", nullptr, buttonStyle()),
	exitButton(Rectangle{}, "EXIT", nullptr, buttonStyle()),
	onStart(std::move(onStart)),
	onQuit(std::move(onQuit)) {}

void MainMenu::refresh()
{
	const int width = GetScreenWidth();
	const int height = GetScreenHeight();

	if (width == screenWidth && height == screenHeight) {
		return;
	}

	screenWidth = width;
	screenHeight = height;

	const Rectangle screen{ 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height) };
	const Vector2 buttonSize{ 200.0f, 60.0f };

	const std::vector<Rectangle> slots = Layout::Column()
		.spacing(30.0f)
		.mainAlign(Layout::Align::Center)
		.crossAlign(Layout::Align::Center)
		.build(screen, { title.getSize(), buttonSize, buttonSize });

	title.setBounds(slots[0]);
	playButton.setBounds(slots[1]);
	exitButton.setBounds(slots[2]);
}

void MainMenu::update(float, NavigationStack& navigation)
{
	refresh();

	playButton.update();
	exitButton.update();

	if (playButton.wasClicked()) {
		navigation.push(makeUnique<AvatarMenu>(onStart));
		return;
	}

	if (exitButton.wasClicked()) {
		onQuit();
	}
}

void MainMenu::render(Engine::Graphics::Renderer& renderer)
{
	refresh();

	renderer.clear(DarkGreen);

	title.render();
	playButton.render();
	exitButton.render();
}