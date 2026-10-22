#pragma once

#include <string>
#include <functional>

namespace Engine {
	struct ApplicationConfig {
		int width = 800;
		int height = 600;
		std::string title = "Game";
		int targetFPS = 60;
		bool resizable = true;
		bool exitOnClose = true;
		bool exitOnEscape = true;
	};

	class Application {
	public:
		Application(const ApplicationConfig& config = ApplicationConfig());
		~Application();

		const ApplicationConfig& getConfig() const;

		int getWidth() const;
		int getHeight() const;

		void run(const std::function<void(float deltaTime)>& frame);
		void stop();

	private:
		ApplicationConfig config;
		bool isRunning = true;
	};
}