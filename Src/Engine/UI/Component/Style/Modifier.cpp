
#include "Modifier.h"

namespace Engine::UI {
    Modifier Modifier::background(Color value) {
        _background = value;
        return *this;
    }

    Modifier Modifier::hover(Color value) {
        _hover = value;
        return *this;
    }

    Modifier Modifier::disabled(bool value) {
        _disabled = value;
        return *this;
    }

    Modifier Modifier::borderColor(Color value) {
        _borderColor = value;
        return *this;
    }

    Modifier Modifier::borderThickness(float value) {
        _borderThickness = value;
        return *this;
    }

    Modifier Modifier::textColor(Color value) {
        _textColor = value;
        return *this;
    }

    Modifier Modifier::fontSize(int value) {
        _fontSize = value;
        return *this;
    }

    Color Modifier::getBackground() const {
        return _background.value_or(BLANK);
    }

    Color Modifier::getHover() const {
        return _hover.value_or(BLANK);
    }

    bool Modifier::getDisabled() const {
        return _disabled.value_or(false);
    }

    Color Modifier::getBorderColor() const {
        return _borderColor.value_or(BLANK);
    }

    float Modifier::getBorderThickness() const {
        return _borderThickness.value_or(0.0f);
    }

    Color Modifier::getTextColor() const {
        return _textColor.value_or(BLACK);
    }

    int Modifier::getFontSize() const {
        return _fontSize.value_or(20);
    }
}