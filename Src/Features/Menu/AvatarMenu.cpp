#include <cctype>
#include <utility>
#include <raylib.h>
#include "AvatarMenu.h"
#include "../Character/Avatar/Soldier.h"
#include "../Character/Avatar/Assasin.h"
#include "../Character/Avatar/Tanker.h"

namespace UI = Engine::UI;

namespace {
	constexpr Color DarkGreen{ 20, 160, 133, 255 };
	constexpr Color Gold{ 255, 203, 0, 255 };

	constexpr float CardWidth = 220.0f;
	constexpr float CardHeight = 320.0f;
	constexpr float CardSpacing = 30.0f;
	constexpr float ButtonWidth = 200.0f;
	constexpr float ButtonHeight = 60.0f;

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

	std::string toUpper(std::string text)
	{
		for (char& c : text) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
		return text;
	}

	// Reads display data from the real avatar class.
	template<typename T>
	AvatarOption makeOption(AvatarVariant variant, Color color)
	{
		const T avatar;
		AvatarOption option{};
		option.variant = variant;
		option.name = toUpper(avatar.getRole());
		option.damage = avatar.getDamage();
		option.armor = avatar.getArmor();
		option.movementSpeed = avatar.getMovementSpeed();
		option.color = color;
		return option;
	}
}

AvatarMenu::AvatarMenu(std::function<void(AvatarVariant)> onStart)
	: options{ {
		makeOption<Soldier>(AvatarVariant::Soldier, Color{ 70, 130, 180, 255 }),
		makeOption<Assasin>(AvatarVariant::Assasin, Color{ 150, 70, 160, 255 }),
		makeOption<Tanker>(AvatarVariant::Tanker,  Color{ 190, 110, 50, 255 })
	} },
	confirmButton(Rectangle{}, "START", nullptr, buttonStyle()),
	backButton(Rectangle{}, "BACK", nullptr, buttonStyle()),
	onStart(std::move(onStart))
{
}

void AvatarMenu::refresh()
{
	const int width = GetScreenWidth();
	const int height = GetScreenHeight();
	if (width == screenWidth && height == screenHeight) return;

	screenWidth = width;
	screenHeight = height;

	const float count = static_cast<float>(options.size());
	const float rowWidth = count * CardWidth + (count - 1.0f) * CardSpacing;
	const float startX = (width - rowWidth) * 0.5f;
	const float cardY = (height - CardHeight) * 0.5f - 20.0f;

	for (size_t i = 0; i < options.size(); ++i) {
		options[i].bounds = Rectangle{ startX + static_cast<float>(i) * (CardWidth + CardSpacing), cardY, CardWidth, CardHeight };
	}

	titleBounds = Rectangle{ 0.0f, cardY - 90.0f, static_cast<float>(width), 50.0f };

	const float buttonsY = cardY + CardHeight + 40.0f;
	const float buttonsWidth = ButtonWidth * 2.0f + CardSpacing;
	const float buttonsX = (width - buttonsWidth) * 0.5f;

	backButton.setBounds(Rectangle{ buttonsX, buttonsY, ButtonWidth, ButtonHeight });
	confirmButton.setBounds(Rectangle{ buttonsX + ButtonWidth + CardSpacing, buttonsY, ButtonWidth, ButtonHeight });
}

void AvatarMenu::update(float, NavigationStack& navigation)
{
	refresh();

	const Vector2 mouse = GetMousePosition();
	hoveredIndex = -1;

	for (size_t i = 0; i < options.size(); ++i) {
		if (CheckCollisionPointRec(mouse, options[i].bounds)) {
			hoveredIndex = static_cast<int>(i);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) selectedIndex = hoveredIndex;
		}
	}

	const int count = static_cast<int>(options.size());
	if (IsKeyPressed(KEY_RIGHT)) selectedIndex = (selectedIndex + 1) % count;
	if (IsKeyPressed(KEY_LEFT))  selectedIndex = (selectedIndex + count - 1) % count;

	confirmButton.update();
	backButton.update();

	if (confirmButton.wasClicked()) {
		onStart(options[selectedIndex].variant);
		return;
	}

	if (backButton.wasClicked()) navigation.pop();   // applied after update returns
}

void AvatarMenu::render(Engine::Graphics::Renderer& renderer)
{
	refresh();

	renderer.clear(DarkGreen);

	const std::string title = "CHOOSE YOUR AVATAR";
	const int titleSize = 50;
	const int titleWidth = renderer.measureText(title, titleSize);
	renderer.drawText(title,
		Vector2{ titleBounds.x + (titleBounds.width - titleWidth) * 0.5f, titleBounds.y }, titleSize, WHITE);

	for (size_t i = 0; i < options.size(); ++i) {
		const AvatarOption& option = options[i];
		const bool selected = static_cast<int>(i) == selectedIndex;
		const bool hovered = static_cast<int>(i) == hoveredIndex;

		renderer.drawRectangle(option.bounds, hovered ? Color{ 225, 250, 244, 255 } : WHITE);

		const Rectangle preview{ option.bounds.x + 20.0f, option.bounds.y + 20.0f, option.bounds.width - 40.0f, 140.0f };
		renderer.drawRectangle(preview, option.color);

		const int nameSize = 28;
		const int nameWidth = renderer.measureText(option.name, nameSize);
		const float nameY = preview.y + preview.height + 15.0f;
		renderer.drawText(option.name,
			Vector2{ option.bounds.x + (option.bounds.width - nameWidth) * 0.5f, nameY }, nameSize, DarkGreen);

		const std::string lines[3] = {
			"DAMAGE  " + std::to_string(static_cast<int>(option.damage)),
			"ARMOR   " + std::to_string(static_cast<int>(option.armor)),
			"SPEED   " + std::to_string(static_cast<int>(option.movementSpeed))
		};

		float lineY = nameY + 40.0f;
		for (const std::string& line : lines) {
			renderer.drawText(line, Vector2{ option.bounds.x + 40.0f, lineY }, 18, DARKGRAY);
			lineY += 24.0f;
		}

		if (selected) renderer.drawRectangleOutline(option.bounds, 6.0f, Gold);
	}

	confirmButton.render();
	backButton.render();
}