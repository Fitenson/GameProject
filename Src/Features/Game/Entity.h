#pragma once

#include <raylib.h>
#include "Engine/Graphics.h"

class Entity {
public:
	Entity(const Vector2& position = Vector2{ 0.0f, 0.0f });
	virtual ~Entity() = default;

	Vector2 getPosition() const;
	void setPosition(Vector2& position);
	void setPosition(const Vector2& position);

	virtual void update(float deltaTime) = 0;
	virtual void render(Engine::Graphics::Renderer& renderer) const = 0;

protected:
	Vector2 position { 0.0f, 0.0f };
};