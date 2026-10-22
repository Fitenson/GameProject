#pragma once

#include <string>
#include <functional>
#include <raylib.h>
#include "../Style/Modifier.h"

namespace Engine::UI {
    class Button {
    public:
        Button(
            const Rectangle& bounds,
            const std::string& label,
            std::function<void()> onClick = nullptr,
            const Modifier& style = Modifier()
        );

        bool getEnabled();
        void setEnabled(bool enabled);

        void setBounds(const Rectangle& bounds);

        bool wasClicked() const;
        void update();
        void render() const;

    private:
        Rectangle bounds{ 0.0f, 0.0f, 0.0f, 0.0f };
        std::string label;
        Modifier style;
        std::function<void()> onClick;
        bool enabled = true;
        bool clicked = false;
    };
}
