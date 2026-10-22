#pragma once

namespace Engine::Layout {
	class Insets {
	public:
		Insets() = default;
		Insets(float left, float top, float right, float bottom);

		static Insets all(float value);
		static Insets symmetric(float horizontal, float vertical);

		float left() const { return _left; }
		float top() const { return _top; }
		float right() const { return _right; }
		float bottom() const { return _bottom; }

		float horizontal() const { return _left + _right; }
		float vertical() const { return _top + _bottom; }

	private:
		float _left = 0.0f;
		float _top = 0.0f;
		float _right = 0.0f;
		float _bottom = 0.0f;
	};
}