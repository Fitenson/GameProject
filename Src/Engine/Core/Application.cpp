#include <raylib.h>
#include <stdexcept>
#include "Application.h"

namespace Engine {
	Application::Application(const ApplicationConfig& config)
		: config(config) {
		unsigned int flags = 0;
		if (config.resizable) {
			flags |= FLAG_WINDOW_RESIZABLE;
		}
		else {
			flags |= FLAG_FULLSCREEN_MODE;
		}

		SetConfigFlags(flags);

		int screenWidth = config.resizable ? config.width : 0;
		int screenHeight = config.resizable ? config.height : 0;

		InitWindow(
			screenWidth,
			screenHeight,
			config.title.c_str()
		);

		if (!IsWindowReady()) {
			throw std::runtime_error("Failed to create window");
		}

		const int fps = config.targetFPS > 0 ? config.targetFPS : GetMonitorRefreshRate(GetCurrentMonitor());
		SetTargetFPS(fps);

		if (!config.exitOnClose || !config.exitOnEscape) {
			SetExitKey(KEY_NULL);
		}
	}

	Application::~Application() {
		if (IsWindowReady()) {
			CloseWindow();
		}
	}

	const ApplicationConfig& Application::getConfig() const {
		return config;
	}

	int Application::getWidth() const {
		return config.width;
	}

	int Application::getHeight() const {
		return config.height;
	}

	void Application::run(const std::function<void(float deltaTime)>& frame) {
		isRunning = true;

		while (isRunning && !(config.exitOnClose && WindowShouldClose())) {
			BeginDrawing();
			ClearBackground(BLACK);

			if (frame) {
				frame(GetFrameTime());
			}

			EndDrawing();
		}

		isRunning = false;
	}

	void Application::stop() {
		isRunning = false;
	}
}