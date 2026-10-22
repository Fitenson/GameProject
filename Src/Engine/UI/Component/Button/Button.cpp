#include "Button.h"

namespace Engine::UI {
    Button::Button(
        const Rectangle& bounds,
        const std::string& label,
        std::function<void()> onClick,
        const Modifier& style
    )
        : bounds(bounds),
        label(label),
        style(style),
        onClick(onClick) {
    }

    bool Button::getEnabled() {
        return enabled;
    }

    void Button::setEnabled(bool enabled) {
        this->enabled = enabled;
    }

    void Button::setBounds(const Rectangle& bounds) {
        this->bounds = bounds;
    }

    void Button::update() {
        clicked = false;

        if (!enabled) {
            return;
        }

        if (!CheckCollisionPointRec(GetMousePosition(), bounds)) {
            return;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            clicked = true;

            if (onClick) {
                onClick();
            }
        }
    }

    bool Button::wasClicked() const {
        return clicked;
    }

    void Button::render() const {
        Color fill = enabled
            ? Color{ 50, 50, 60, 255 }
        : Color{ 100, 100, 100, 255 };

        DrawRectangleRec(bounds, fill);

        const int fontSize = style.getFontSize();
        const int textWidth = MeasureText(label.c_str(), fontSize);

        DrawText(
            label.c_str(),
            static_cast<int>(bounds.x + (bounds.width - textWidth) / 2.0f),
            static_cast<int>(bounds.y + (bounds.height - fontSize) / 2.0f),
            fontSize,
            style.getTextColor()
        );
    }
}
