#pragma once

namespace Engine::Layout {
	enum class Align { Start, Center, End };

	float alignOffset(Align align, float freeSpace);
}