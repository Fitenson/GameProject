#include "Magazine.h"

Magazine::Magazine(float magazineLength, int capacity, float reloadSpeedMultiplier)
	: magazineLength(magazineLength),
	capacity(std::max(1, capacity)),
	reloadSpeedMultiplier(std::max(0.1f, reloadSpeedMultiplier)) {
	rounds.setMax(this->capacity);
	refill();
}

// Mounted under the receiver, so it adds nothing along the barrel axis.
float Magazine::length() const { return 0.0f; }

void Magazine::modify(GunStats& stats) const
{
	stats.magazineSize += capacity;
	stats.reloadSpeed *= reloadSpeedMultiplier;
}

Bullet* Magazine::fire() {
	auto result = rounds.dequeue();

	if (result.has_value()) {
		Bullet* bullet = result.value();
		return bullet;
	}

	return nullptr;
}

void Magazine::refill()
{
	while (!rounds.isFull()) {
		rounds.enqueue(new Bullet(Vector2{ 0.0f, 0.0f }, Vector2{ 1.0f, 0.0f }, 0.0f, 0.0f, 0.0f));
	}
}
void Magazine::swapQueue(Engine::DataStructure::Queue<Bullet*>& other) { std::swap(rounds, other); }

bool Magazine::isEmpty() const { return rounds.isEmpty(); }
bool Magazine::isFull() const { return rounds.isFull(); }
int Magazine::getLoaded() const { return static_cast<int>(rounds.size()); }
int Magazine::getCapacity() const { return capacity; }

void Magazine::render(Engine::Graphics::Renderer& renderer, const GunMount& mount) const
{
	// A bigger magazine is drawn longer, scaled down so it fits the top-down view.
	const float from = mount.getOffset() + 2.0f;
	const float to = from + magazineLength * 0.4f;
	const float side = -6.0f;   // opposite side from the scope (+6)

	const Vector2 a = mount.pointAt(from, side);
	const Vector2 b = mount.pointAt(to, side);

	renderer.drawLine(a, b, 6.0f, Color{ 45, 48, 58, 255 });          // body
	renderer.drawLine(a, b, 2.0f, Color{ 90, 95, 110, 255 });         // highlight
	renderer.drawLine(mount.pointAt(to, side - 3.0f),                 // base plate
		mount.pointAt(to, side + 3.0f), 2.5f, Color{ 150, 120, 50, 255 });
}
