#include "CameraController.h"

namespace Engine::Graphics {
	CameraController::CameraController()
	{
		camera.target = Vector2{ 0.0f, 0.0f };
		camera.offset = Vector2{ 0.0f, 0.0f };
		camera.rotation = 0.0f;
		camera.zoom = 1.0f;
	}

	Camera2D& CameraController::getCamera() {
		return camera;
	}

	const Camera2D& CameraController::getCamera() const {
		return camera;
	}

	Vector2 CameraController::getPosition() const {
		return camera.target;
	}

	void CameraController::setPosition(Vector2& position) {
		camera.target = position;
	}

	void CameraController::setPosition(const Vector2& position) {
		camera.target = position;
	}

	void CameraController::follow(const Vector2& target) {
		camera.target = target;
	}

	void CameraController::refreshViewport() {
		camera.offset = Vector2{ GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f };
	}

	void CameraController::begin() const {
		BeginMode2D(camera);
	}

	void CameraController::end() const {
		EndMode2D();
	}

	Vector2 CameraController::screenToWorld2D(const Vector2& screen) const {
		return GetScreenToWorld2D(screen, camera);
	}

	void CameraController::setZoom(float zoom) { camera.zoom = zoom; }
}