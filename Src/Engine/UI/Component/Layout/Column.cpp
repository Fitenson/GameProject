
#include "Column.h"

#include <algorithm>

namespace Engine::Layout {
    Column Column::spacing(float value) {
        _spacing = value;
        return *this;
    }

    Column Column::padding(const Insets& value) {
        _padding = value;
        return *this;
    }

    Column Column::mainAlign(Align value) {
        _mainAlign = value;
        return *this;
    }

    Column Column::crossAlign(Align value) {
        _crossAlign = value;
        return *this;
    }

    std::vector<Rectangle> Column::build(
        const Rectangle& area,
        const std::vector<Vector2>& childSizes
    ) const {
        std::vector<Rectangle> result;
        result.reserve(childSizes.size());

        if (childSizes.empty()) {
            return result;
        }

        const float x = area.x + _padding.left();
        const float y = area.y + _padding.top();

        const float width = std::max(
            0.0f,
            area.width - _padding.horizontal()
        );

        const float height = std::max(
            0.0f,
            area.height - _padding.vertical()
        );

        float total = _spacing * static_cast<float>(childSizes.size() - 1);

        for (const Vector2& size : childSizes) {
            total += size.y;
        }

        const float freeHeight = std::max(
            0.0f,
            height - total
        );

        float cursor = y + alignOffset(_mainAlign, freeHeight);

        for (const Vector2& size : childSizes) {
            const float childX =
                x + alignOffset(_crossAlign, width - size.x);

            result.push_back({
                childX,
                cursor,
                size.x,
                size.y
                });

            cursor += size.y + _spacing;
        }

        return result;
    }

    std::vector<Rectangle> Column::build(
        const Rectangle& area,
        int count,
        Vector2 childSize
    ) const {
        const std::size_t n =
            static_cast<std::size_t>(std::max(count, 0));

        return build(
            area,
            std::vector<Vector2>(n, childSize)
        );
    }
}