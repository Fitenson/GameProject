
#pragma once

#include <optional>
#include <raylib.h>

namespace Engine::UI {
    class Modifier {
    public:
        Modifier background(Color value);
        Modifier hover(Color value);
        Modifier disabled(bool value);
        Modifier borderColor(Color value);
        Modifier borderThickness(float value);
        Modifier textColor(Color value);
        Modifier fontSize(int value);

        Color getBackground() const;
        Color getHover() const;
        bool getDisabled() const;
        Color getBorderColor() const;
        float getBorderThickness() const;
        Color getTextColor() const;
        int getFontSize() const;

    private:
        std::optional<Color> _background;
        std::optional<Color> _hover;
        std::optional<bool> _disabled;
        std::optional<Color> _borderColor;
        std::optional<float> _borderThickness;
        std::optional<Color> _textColor;
        std::optional<int> _fontSize;
    };
}