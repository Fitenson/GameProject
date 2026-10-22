// Dialog.cpp
#include <utility>
#include "Dialog.h"

namespace Engine::UI {
	namespace {
		constexpr float PanelWidth = 440.0f;
		constexpr float Padding = 28.0f;
		constexpr int TitleSize = 40;
		constexpr int MessageSize = 20;
		constexpr float ButtonWidth = 300.0f;
		constexpr float ButtonHeight = 54.0f;
		constexpr float ButtonGap = 14.0f;
		constexpr float ArmDelay = 0.35f;

		Modifier buttonStyle()
		{
			return Modifier()
				.background(WHITE)
				.hover(Color{ 225, 250, 244, 255 })
				.textColor(Color{ 20, 160, 133, 255 })
				.fontSize(26)
				.borderThickness(0.0f)
				.borderColor(BLANK);
		}
	}

	Dialog::Dialog(std::string title, std::string message)
		: title(std::move(title)), message(std::move(message)) {
	}

	int Dialog::addButton(const std::string& label)
	{
		buttons.push_back(makeUnique<Button>(Rectangle{}, label, nullptr, buttonStyle()));
		return buttons.size() - 1;
	}

	void Dialog::layout()
	{
		const float width = static_cast<float>(GetScreenWidth());
		const float height = static_cast<float>(GetScreenHeight());
		const float count = static_cast<float>(buttons.size());

		const float titleBlock = TitleSize + 18.0f;
		const float messageBlock = message.empty() ? 0.0f : MessageSize + 22.0f;
		const float buttonsBlock = count > 0.0f ? count * ButtonHeight + (count - 1.0f) * ButtonGap : 0.0f;
		const float panelHeight = Padding * 2.0f + titleBlock + messageBlock + buttonsBlock;

		panel = Rectangle{ (width - PanelWidth) * 0.5f, (height - panelHeight) * 0.5f, PanelWidth, panelHeight };

		float y = panel.y + Padding + titleBlock + messageBlock;
		for (UniquePtr<Button>& button : buttons) {
			button->setBounds(Rectangle{ panel.x + (PanelWidth - ButtonWidth) * 0.5f, y, ButtonWidth, ButtonHeight });
			y += ButtonHeight + ButtonGap;
		}
	}

	int Dialog::update(float deltaTime)
	{
		age += deltaTime;
		layout();

		if (age < ArmDelay) return -1;

		for (UniquePtr<Button>& button : buttons) button->update();

		for (size_t i = 0; i < buttons.size(); ++i) {
			if (buttons[i]->wasClicked()) return static_cast<int>(i);
		}
		return -1;
	}

	void Dialog::render(Engine::Graphics::Renderer& renderer)
	{
		layout();

		// Dim everything behind the dialog.
		renderer.drawRectangle(
			Rectangle{ 0.0f, 0.0f, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) },
			Color{ 0, 0, 0, 160 });

		// Panel with a soft shadow.
		renderer.drawRectangle(Rectangle{ panel.x + 5.0f, panel.y + 6.0f, panel.width, panel.height }, Color{ 0, 0, 0, 90 });
		renderer.drawRectangle(panel, Color{ 24, 44, 50, 250 });
		renderer.drawRectangleOutline(panel, 3.0f, Color{ 20, 160, 133, 255 });

		float y = panel.y + Padding;

		const int titleWidth = renderer.measureText(title, TitleSize);
		renderer.drawText(title, Vector2{ panel.x + (panel.width - titleWidth) * 0.5f, y }, TitleSize, WHITE);
		y += TitleSize + 18.0f;

		if (!message.empty()) {
			const int messageWidth = renderer.measureText(message, MessageSize);
			renderer.drawText(message, Vector2{ panel.x + (panel.width - messageWidth) * 0.5f, y }, MessageSize, Color{ 200, 215, 215, 255 });
		}

		for (UniquePtr<Button>& button : buttons) button->render();
	}
}