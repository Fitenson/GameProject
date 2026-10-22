
#pragma once

#include <vector>
#include <algorithm>
#include <raylib.h>

#include "../Style/Insets.h"
#include "../Style/Align.h"

namespace Engine::Layout {
    class Row {
    public:
        Row spacing(float value);
        Row padding(const Insets& value);
        Row mainAlign(Align value);
        Row crossAlign(Align value);

        std::vector<Rectangle> build(
            const Rectangle& area,
            const std::vector<Vector2>& childSizes
        ) const;

        std::vector<Rectangle> build(
            const Rectangle& area,
            int count,
            Vector2 childSize
        ) const;

    private:
        float _spacing = 0.0f;
        Insets _padding;
        Align _mainAlign;
        Align _crossAlign;
    };
}