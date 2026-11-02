#pragma once

#include "Engine/DataStructure.h"
#include "GunPart.h"
#include "../Bullet.h"

class Magazine : public GunPart {
public:
	explicit Magazine(
		float magazineLength = 30.0f,
		int capacity = 30,
		float reloadSpeedMultiplier = 1.0f
	);

	float length() const override;

	void modify(GunStats& stats) const override;

	Bullet* fire();
	void refill();
	void swapQueue(Engine::DataStructure::Queue<Bullet*>& other);
	bool isEmpty() const;
	bool isFull() const;
	int getLoaded() const;
	int getCapacity() const;

	void render(
		Engine::Graphics::Renderer& renderer,
		const GunMount& mount
	) const override;

private:
	float magazineLength;
	int capacity;
	float reloadSpeedMultiplier;
	Engine::DataStructure::Queue<Bullet*> rounds;	//	Store bullets
};