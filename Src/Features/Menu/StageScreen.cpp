#include <string>
#include <utility>
#include "StageScreen.h"

namespace UI = Engine::UI;

namespace {
	constexpr Color DarkGreen{ 20, 160, 133, 255 };
	constexpr Color Gold{ 255, 203, 0, 255 };
	constexpr int Columns = 5;
	constexpr float CardW = 210.0f, CardH = 150.0f, Gap = 20.0f;

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

StageScreen::StageScreen(int unlockedStages, std::function<void(int)> onSelect)
	: unlocked(unlockedStages), onSelect(std::move(onSelect)),
	backButton(Rectangle{}, "BACK", nullptr, buttonStyle())
{
	for (int i = 0; i < StageConfig::Count; ++i) stages[static_cast<size_t>(i)] = StageConfig::forStage(i + 1);
}

Rectangle StageScreen::cardBounds(int index) const
{
	const int rows = (StageConfig::Count + Columns - 1) / Columns;
	const float totalW = Columns * CardW + (Columns - 1) * Gap;
	const float totalH = rows * CardH + (rows - 1) * Gap;
	const float startX = (static_cast<float>(screenWidth) - totalW) * 0.5f;
	const float startY = (static_cast<float>(screenHeight) - totalH) * 0.5f - 10.0f;

	return Rectangle{ startX + (index % Columns) * (CardW + Gap), startY + (index / Columns) * (CardH + Gap), CardW, CardH };
}

void StageScreen::layout()
{
	screenWidth = GetScreenWidth();
	screenHeight = GetScreenHeight();

	const Rectangle last = cardBounds(StageConfig::Count - 1);
	backButton.setBounds(Rectangle{ (static_cast<float>(screenWidth) - 200.0f) * 0.5f, last.y + last.height + 40.0f, 200.0f, 60.0f });
}

void StageScreen::update(float, NavigationStack& navigation)
{
	layout();

	const Vector2 mouse = GetMousePosition();
	hovered = -1;

	for (int i = 0; i < StageConfig::Count; ++i) {
		if (!CheckCollisionPointRec(mouse, cardBounds(i))) continue;
		hovered = i;

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && i < unlocked) {
			onSelect(i + 1);
			return;
		}
	}

	backButton.update();
	if (backButton.wasClicked()) navigation.pop();
}

void StageScreen::render(Engine::Graphics::Renderer& renderer)
{
	layout();
	renderer.clear(DarkGreen);

	const std::string title = "SELECT STAGE";
	const Rectangle first = cardBounds(0);
	renderer.drawText(title, Vector2{ (static_cast<float>(screenWidth) - renderer.measureText(title, 50)) * 0.5f, first.y - 80.0f }, 50, WHITE);

	for (int i = 0; i < StageConfig::Count; ++i) {
		const StageConfig& cfg = stages[static_cast<size_t>(i)];
		const Rectangle card = cardBounds(i);
		const bool locked = i >= unlocked;

		renderer.drawRectangle(card, locked ? Color{ 150, 165, 165, 255 }
		: (hovered == i ? Color{ 225, 250, 244, 255 } : WHITE));

		const std::string head = "STAGE " + std::to_string(cfg.number);
		renderer.drawText(head, Vector2{ card.x + (card.width - renderer.measureText(head, 26)) * 0.5f, card.y + 12.0f }, 26,
			locked ? DARKGRAY : DarkGreen);

		if (locked) {
			const std::string text = "LOCKED";
			renderer.drawText(text, Vector2{ card.x + (card.width - renderer.measureText(text, 24)) * 0.5f, card.y + 70.0f }, 24, DARKGRAY);
		}
		else {
			const std::string lines[4] = {
				"Map    " + std::to_string(cfg.mapCells) + " x " + std::to_string(cfg.mapCells),
				"Enemy  Lv " + std::to_string(cfg.enemy.level),
				"Goal   " + std::to_string(cfg.killTarget) + " kills",
				"Gates  " + std::to_string(cfg.puzzles.size())
			};
			float y = card.y + 52.0f;
			for (const std::string& line : lines) {
				renderer.drawText(line, Vector2{ card.x + 22.0f, y }, 18, DARKGRAY);
				y += 24.0f;
			}
		}

		if (hovered == i && !locked) renderer.drawRectangleOutline(card, 5.0f, Gold);
	}

	backButton.render();
}