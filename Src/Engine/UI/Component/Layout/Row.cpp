
#include "Row.h"

namespace Engine::Layout {
    Row Row::spacing(float value) {
        _spacing = value;
        return *this;
    }

    Row Row::padding(const Insets& value) {
        _padding = value;
        return *this;
    }

    Row Row::mainAlign(Align value) {
        _mainAlign = value;
        return *this;
    }

    Row Row::crossAlign(Align value) {
        _crossAlign = value;
        return *this;
    }

    std::vector<Rectangle> Row::build(
        const Rectangle& area,
        const std::vector<Vector2>& childSizes
    ) const {
        std::vector<Rectangle> result;
        result.reserve(childSizes.size());

        if (childSizes.empty()) {
            return result;
        }

        // Area remaining after padding.
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

        // Total width of children and gaps.
        float total = _spacing *
            static_cast<float>(childSizes.size() - 1);

        for (const Vector2& size : childSizes) {
            total += size.x;
        }

        const float freeWidth = std::max(0.0f, width - total);

        float cursor = x + alignOffset(_mainAlign, freeWidth);

        for (const Vector2& size : childSizes) {
            const float childY =
                y + alignOffset(_crossAlign, height - size.y);

            result.push_back({
                cursor,
                childY,
                size.x,
                size.y
                });

            cursor += size.x + _spacing;
        }

        return result;
    }

    std::vector<Rectangle> Row::build(
        const Rectangle& area,
        int count,
        Vector2 childSize
    ) const {
        const std::size_t n =
            static_cast<std::size_t>(std::max(count, 0));

        return build(area, std::vector<Vector2>(n, childSize));
    }
}