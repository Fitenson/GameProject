#pragma once

#include <string>
#include <raylib.h>
#include "../Style/Modifier.h"

namespace Engine::UI {
	class Label {
	public:
		Label() = default;
		Label(const std::string& text, const Modifier& modifier = Modifier());

		void setBounds(const Rectangle& bounds);

		Vector2 getSize() const;
		void render() const;

	private:
		Rectangle bounds{};
		std::string text;
		Modifier modifier;
	};
}