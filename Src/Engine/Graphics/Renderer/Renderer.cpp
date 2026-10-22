#include "Renderer.h"

namespace Engine::Graphics {
	void Renderer::clear(Color color) { ClearBackground(color); }

	void Renderer::beginWorld(const Camera2D& camera) {
		this->camera = camera;
		inWorld = true;
		BeginMode2D(camera);
	}

	void Renderer::endWorld() {
		EndMode2D();
		inWorld = false;
	}

	void Renderer::drawRectangle(const Rectangle& rectangle, Color color) {
		DrawRectangleRec(rectangle, color);
	}

	void Renderer::drawRectangleOutline(const Rectangle& rectangle, float thickness, Color color) {
		DrawRectangleLinesEx(rectangle, thickness, color);
	}

	void Renderer::drawText(const std::string& text, Vector2 position, int fontSize, Color color) {
		DrawText(text.c_str(), static_cast<int>(position.x), static_cast<int>(position.y), fontSize, color);
	}

	int Renderer::measureText(const std::string& text, int fontSize) const {
		return MeasureText(text.c_str(), fontSize);
	}

	void Renderer::drawCircle(Vector2 center, float radius, Color color) { 
		DrawCircleV(center, radius, color); 
	}

	void Renderer::drawCircleOutline(Vector2 center, float radius, Color color) { 
		DrawCircleLinesV(center, radius, color);
	}

	void Renderer::drawLine(Vector2 start, Vector2 end, float thickness, Color color) {
		DrawLineEx(start, end, thickness, color);
	}

	Rectangle Renderer::getViewRectangle() const {
		const float width = static_cast<float>(GetScreenWidth());
		const float height = static_cast<float>(GetScreenHeight());

		if (!inWorld) {
			return Rectangle{ 0.0f, 0.0f, width, height };
		}

		const Vector2 topLeft = GetScreenToWorld2D(Vector2{ 0.0f, 0.0f }, camera);
		const Vector2 bottomRight = GetScreenToWorld2D(Vector2{ width, height }, camera);
		return Rectangle{ topLeft.x, topLeft.y, bottomRight.x - topLeft.x, bottomRight.y - topLeft.y };
	}

	bool Renderer::isVisible(const Rectangle& rectangle) const {
		return CheckCollisionRecs(getViewRectangle(), rectangle);
	}
}