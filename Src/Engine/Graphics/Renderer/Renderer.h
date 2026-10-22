#pragma once

#include <string>
#include <raylib.h>

namespace Engine::Graphics {
	class Renderer {
	public:
		void clear(Color color);

		void beginWorld(const Camera2D& camera);
		void endWorld();

		void drawRectangle(const Rectangle& rectangle, Color color);
		void drawRectangleOutline(const Rectangle& rectangle, float thickness, Color color);

		void drawText(const std::string& text, Vector2 position, int fontSize, Color color);
		int measureText(const std::string& text, int fontSize) const;

		void drawCircle(Vector2 center, float radius, Color color);
		void drawCircleOutline(Vector2 center, float radius, Color color);

		void drawLine(Vector2 start, Vector2 end, float thickness, Color color);

		Rectangle getViewRectangle() const;
		bool isVisible(const Rectangle& rectangle) const;

	private:
		Camera2D camera{};
		bool inWorld = false;
	};
}