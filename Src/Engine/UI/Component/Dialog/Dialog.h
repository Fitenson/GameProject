// Dialog.h
#pragma once

#include <string>
#include <vector>
#include <raylib.h>
#include "Engine/Core.h"
#include "Engine/Graphics.h"
#include "../Button/Button.h"

namespace Engine::UI {
	class Dialog {
	public:
		explicit Dialog(std::string title, std::string message = "");

		// Returns the index of the new button.
		int addButton(const std::string& label);

		// Returns the index of the button clicked this frame, or -1.
		// Input is ignored for a short moment after the dialog opens.
		int update(float deltaTime);

		void render(Engine::Graphics::Renderer& renderer);

	private:
		std::string title;
		std::string message;
		std::vector<UniquePtr<Button>> buttons;
		Rectangle panel{};
		float age = 0.0f;

		void layout();
	};
}