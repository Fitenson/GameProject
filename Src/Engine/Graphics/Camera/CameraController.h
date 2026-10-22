#pragma once

#include <raylib.h>

namespace Engine::Graphics {
	class CameraController {
	public:
		CameraController();

		Camera2D& getCamera();
		const Camera2D& getCamera() const;

		Vector2 getPosition() const;
		void setPosition(Vector2& position);
		void setPosition(const Vector2& position);

		void follow(const Vector2& target);
		void begin() const;
		void end() const;
		void refreshViewport();
		void setZoom(float zoom);
		Vector2 screenToWorld2D(const Vector2& screen) const;

	private:
		Camera2D camera{};
	};
}