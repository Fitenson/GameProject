// ItemSpawner.cpp
#include <cmath>
#include <string>
#include <variant>
#include <type_traits>
#include "ItemSpawner.h"
#include "../World/Map/Map.h"
#include "../Character/Weapon/Weapon.h"

namespace
{
	Color colorOf(const Item& item)
	{
		return std::visit(
			[](const auto& attachment) -> Color
			{
				using T = std::decay_t<decltype(attachment)>;

				if constexpr (std::is_same_v<T, GunAttachment>)
				{
					switch (attachment)
					{
					case GunAttachment::Scope:
						return Color{ 90, 170, 255, 255 };

					case GunAttachment::Compensator:
						return Color{ 245, 200, 60, 255 };

					case GunAttachment::Suppressor:
						return Color{ 175, 95, 225, 255 };

					default:
						return WHITE;
					}
				}
				else
				{
					// Sword attachment colors.
					return WHITE;
				}
			},
			item.attachment.attachment
		);
	}
}

Vector2 ItemSpawner::pickSpawnPoint(const Vector2& center, float viewRadius, const Map& map) const
{
	const Rectangle bounds = map.getPixelBounds();
	const float margin = 24.0f;

	for (int attempt = 0; attempt < 30; ++attempt) {
		const float angle = GetRandomValue(0, 35999) / 100.0f * DEG2RAD;
		const float distance = viewRadius * (0.35f + 0.55f * GetRandomValue(0, 100) / 100.0f);
		const Vector2 p{ center.x + std::cos(angle) * distance, center.y + std::sin(angle) * distance };

		const bool inMap = p.x > bounds.x + margin && p.x < bounds.x + bounds.width - margin &&
			p.y > bounds.y + margin && p.y < bounds.y + bounds.height - margin;
		if (inMap && !map.isBlocked(p)) return p;
	}
	return center;   // the player stands on open ground
}


void ItemSpawner::spawn(
	const Vector2& center,
	float viewRadius,
	const Map& map)
{
	const int kind = GetRandomValue(0, 1);

	Attachment attachment;

	if (kind == 0)
	{
		const int gunKind = GetRandomValue(0, 2);
		attachment.attachment = static_cast<GunAttachment>(gunKind);
	}
	else
	{
		const int swordKind = GetRandomValue(0, 2);
		attachment.attachment = static_cast<SwordAttachment>(swordKind);
	}

	items.push_back(Item{
		pickSpawnPoint(center, viewRadius, map),
		attachment
		});
}


std::vector<Item> ItemSpawner::collect(const Vector2& playerPosition, float reach)
{
	std::vector<Item> taken;

	std::erase_if(items, [&](const Item& item) {
		const float dx = item.position.x - playerPosition.x;
		const float dy = item.position.y - playerPosition.y;
		if (dx * dx + dy * dy > reach * reach) return false;
		taken.push_back(item);
		return true;
		});

	return taken;
}

void ItemSpawner::render(Engine::Graphics::Renderer& renderer, const Vector2& viewCenter, float viewRadius) const
{
	const float time = static_cast<float>(GetTime());

	for (const Item& item : items) {
		const float dx = item.position.x - viewCenter.x;
		const float dy = item.position.y - viewCenter.y;
		if (dx * dx + dy * dy > viewRadius * viewRadius) continue;

		const Color color = colorOf(item);
		const float bob = std::sin(time * 3.0f) * 2.0f;
		const Vector2 c{ item.position.x, item.position.y + bob };

		renderer.drawCircle(c, 14.0f + std::sin(time * 4.0f) * 2.0f, Color{ color.r, color.g, color.b, 50 });   // glow
		renderer.drawCircle(Vector2{ c.x + 1.5f, c.y + 9.0f }, 6.0f, Color{ 0, 0, 0, 70 });                     // shadow

		const Rectangle crate{ c.x - 7.0f, c.y - 7.0f, 14.0f, 14.0f };
		renderer.drawRectangle(crate, Color{ 55, 60, 72, 255 });
		renderer.drawRectangleOutline(crate, 2.0f, color);
		renderer.drawCircle(c, 3.0f, color);

		const std::string label = Weapon::getAttachmentName(item.attachment);
		renderer.drawText(label, Vector2{ c.x - renderer.measureText(label, 10) * 0.5f, c.y - 22.0f }, 10, WHITE);
	}
}

void ItemSpawner::clear() { items.clear(); }

int ItemSpawner::count() const { return items.size(); }