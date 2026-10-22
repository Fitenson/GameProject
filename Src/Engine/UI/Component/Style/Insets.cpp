
#include "Insets.h"

namespace Engine::Layout {
    Insets::Insets(
        float left,
        float top,
        float right,
        float bottom
    ) : _left(left),
        _top(top),
        _right(right),
        _bottom(bottom) {
    }

    Insets Insets::all(float value) {
        return Insets(value, value, value, value);
    }

    Insets Insets::symmetric(float horizontal, float vertical) {
        return Insets(horizontal, vertical, horizontal, vertical);
    }
}