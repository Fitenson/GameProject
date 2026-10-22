#include "Label.h"

namespace Engine::UI {
	Label::Label(const std::string& text, const Modifier& modifier)
	: text(text), modifier(modifier) {}

	void Label::setBounds(const Rectangle& bounds) { this->bounds = bounds; }

	Vector2 Label::getSize() const {
		const int fontSize = modifier.getFontSize();

		return Vector2{
			static_cast<float>(MeasureText(text.c_str(), fontSize)),
			static_cast<float>(fontSize)
		};
	}

    void Label::render() const {
        const int fontSize = modifier.getFontSize();
        const Vector2 size = getSize();

        const int x = static_cast<int>(
            bounds.x + (bounds.width - size.x) * 0.5f
            );

        const int y = static_cast<int>(
            bounds.y + (bounds.height - size.y) * 0.5f
            );

        DrawText(
            text.c_str(),
            x,
            y,
            fontSize,
            modifier.getTextColor()
        );
    }
}