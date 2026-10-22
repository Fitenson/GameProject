#include <algorithm>
#include <utility>
#include "Gun.h"
#include "GunParts.h"

Gun::Gun() : parts(nullptr), tail(nullptr), Weapon("Gun", 0.15f, 1.0f, true)
{
	addPart(makeUnique<Receiver>());
	addPart(makeUnique<Barrel>());
}

Gun::Gun(const std::string& name, float cooldown, float damageMultiplier)
	: parts(nullptr), tail(nullptr), Weapon(name, cooldown, damageMultiplier, true) {
}

Iterator<GunPart> Gun::begin() const { return Iterator<GunPart>(parts.get()); }
Iterator<GunPart> Gun::end() const { return Iterator<GunPart>(); }

void Gun::addPart(UniquePtr<GunPart> part)
{
	//	Check if the part is a null
	if (part == nullptr) return;

	GunPart* newPart = part.get();

	//	If the linked list is empty, the new part becomes the head
	if (tail == nullptr) {
		parts = std::move(part);
	}
	else {
		//	Else, link the new part with the current tail
		tail->setNext(std::move(part));
	}

	tail = newPart;
	rebuild();
}

// Recompute the stats and the tip position from the chain.
void Gun::rebuild()
{
	stats = GunStats{};
	tipOffset = HolderGap;

	for (const GunPart* part = parts.get(); part; part = part->getNext()) {
		part->modify(stats);
		tipOffset += part->length();
	}
}

void Gun::onAttack(const AttackContext& context)
{
	flashTimer = 0.05f;
	recoil = stats.recoil;

	const Vector2 tip{
		context.playerPosition.x + context.direction.x * tipOffset,
		context.playerPosition.y + context.direction.y * tipOffset
	};

	bullets.emplace_back(tip, context.direction, stats.bulletSpeed, damageOf(context), stats.bulletRange);
}

void Gun::onUpdate(float deltaTime, const AttackContext& context)
{
	flashTimer = std::max(0.0f, flashTimer - deltaTime);
	recoil = std::max(0.0f, recoil - 80.0f * deltaTime);

	for (Bullet& bullet : bullets) {
		bullet.update(deltaTime);
		if (!bullet.isAlive()) continue;

		const Vector2 b = bullet.getPosition();
		for (Enemy* target : context.targets) {
			if (target->isDead()) continue;

			const Vector2 t = target->getHitPosition();
			const float reach = target->getHitRadius() + Bullet::radius;
			const float dx = t.x - b.x;
			const float dy = t.y - b.y;

			if (dx * dx + dy * dy <= reach * reach) {
				target->takeDamage(bullet.getDamage());
				bullet.kill();
				break;
			}
		}
	}

	std::erase_if(bullets, [](const Bullet& b) { return !b.isAlive(); });
}

void Gun::renderHeld(Engine::Graphics::Renderer& renderer, const Vector2& playerPosition, const Vector2& direction) const
{
	// Each part begins where the previous one ended.
	float offset = HolderGap;
	for (const GunPart& part : *this) {
		part.render(renderer, GunMount{ playerPosition, direction, offset, recoil });
		offset += part.length();
	}

	if (flashTimer > 0.0f && stats.flashScale > 0.0f) {
		const GunMount tipMount{ playerPosition, direction, tipOffset, recoil };
		const Vector2 flash = tipMount.pointAt(tipOffset + 4.0f);
		renderer.drawCircle(flash, 9.0f * stats.flashScale, Color{ 255, 170, 40, 255 });
		renderer.drawCircle(flash, 5.0f * stats.flashScale, Color{ 255, 245, 180, 255 });
	}
}

void Gun::renderEffects(Engine::Graphics::Renderer& renderer) const
{
	for (const Bullet& bullet : bullets) {
		bullet.render(renderer);
	}
}