// World.h
#pragma once

#include <string>
#include <vector>
#include <raylib.h>
#include "Engine/Graphics.h"
#include "./Map/Map.h"
#include "./Map/Stage.h"
#include "../Spawner/EnemySpawner.h"
#include "../Character/Combat/AttackContext.h"

class World {
public:
	explicit World(const StageConfig& config);

	static constexpr float exploreRadius = 300.0f;

	Map& getMap();
	const Map& getMap() const;
	Stage& getStage();
	const Stage& getStage() const;
	EnemySpawner& getSpawner();

	Vector2 getFocus() const;
	void setFocus(const Vector2& focus);

	Engine::Graphics::CameraController& getCamera();
	const Engine::Graphics::CameraController& getCamera() const;

	void update(float deltaTime, const Vector2& playerPosition, std::vector<Target>& targets);
	void drawChunks(Engine::Graphics::Renderer& renderer) const;
	void drawEntities(Engine::Graphics::Renderer& renderer) const;
	void collectTargets(std::vector<Enemy*>& out);

private:
	Map map;
	Engine::Graphics::CameraController camera;
	EnemySpawner spawner;
	Stage stage;
	Vector2 exploreCenter{ 0.0f, 0.0f };

	static constexpr float deadzone = 0.1f;

	void followDeadzone(Engine::Graphics::CameraController& camera, const Vector2& target);
};