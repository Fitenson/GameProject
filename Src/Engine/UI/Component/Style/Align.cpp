#include "Align.h"

namespace Engine::Layout {
    float alignOffset(Align align, float freeSpace) {
        switch (align) {
        case Align::Center:
            return freeSpace * 0.5f;

        case Align::End:
            return freeSpace;

        default:
            return 0.0f;
        }
    }
}