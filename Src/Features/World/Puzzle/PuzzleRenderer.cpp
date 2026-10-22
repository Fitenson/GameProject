#include <algorithm>
#include <cmath>
#include "PuzzleRenderer.h"

namespace {
	using Engine::Graphics::Renderer;
	constexpr float Pi = 3.14159265f;

	Color runeColor(int index, unsigned char alpha = 255)
	{
		static const Color colors[4] = {
			{ 225, 70, 60, 255 }, { 70, 130, 235, 255 }, { 240, 200, 60, 255 }, { 175, 95, 225, 255 }
		};
		Color c = colors[((index % 4) + 4) % 4];
		c.a = alpha;
		return c;
	}

	bool inside(const Vector2& p, const Vector2& c, float r)
	{
		const float dx = p.x - c.x, dy = p.y - c.y;
		return dx * dx + dy * dy <= r * r;
	}

	// A partial ring starting at the top, drawn clockwise.
	void arc(Renderer& r, Vector2 c, float radius, float fraction, float thickness, Color color)
	{
		const int steps = std::max(1, static_cast<int>(24.0f * fraction));
		for (int i = 0; i < steps; ++i) {
			const float a0 = -Pi * 0.5f + 2.0f * Pi * fraction * i / steps;
			const float a1 = -Pi * 0.5f + 2.0f * Pi * fraction * (i + 1) / steps;
			r.drawLine(Vector2{ c.x + std::cos(a0) * radius, c.y + std::sin(a0) * radius },
				Vector2{ c.x + std::cos(a1) * radius, c.y + std::sin(a1) * radius }, thickness, color);
		}
	}

	void drawLever(Renderer& r, Vector2 c, float cell, bool on)
	{
		const float baseY = c.y + cell * 0.5f;

		const Rectangle plate{ c.x - 8.0f, baseY - 6.0f, 16.0f, 7.0f };
		r.drawRectangle(Rectangle{ plate.x + 1.5f, plate.y + 2.0f, plate.width, plate.height }, Color{ 15, 30, 15, 90 });
		r.drawRectangle(plate, Color{ 88, 92, 100, 255 });
		r.drawRectangle(Rectangle{ plate.x, plate.y, plate.width, 2.0f }, Color{ 135, 140, 150, 255 });
		r.drawRectangleOutline(plate, 1.0f, Color{ 40, 42, 48, 255 });
		r.drawRectangle(Rectangle{ c.x - 5.0f, baseY - 4.0f, 10.0f, 2.0f }, Color{ 28, 28, 32, 255 });

		const Vector2 pivot{ c.x, baseY - 3.0f };
		const float angle = on ? 0.65f : -0.65f;
		const Vector2 tip{ pivot.x + std::sin(angle) * 13.0f, pivot.y - std::cos(angle) * 13.0f };
		const Color knob = on ? Color{ 80, 230, 110, 255 } : Color{ 225, 70, 60, 255 };

		if (on) r.drawCircle(tip, 7.0f, Color{ 80, 230, 110, 60 });
		r.drawLine(pivot, tip, 3.0f, Color{ 150, 154, 164, 255 });
		r.drawLine(pivot, tip, 1.0f, Color{ 215, 218, 226, 255 });
		r.drawCircle(tip, 3.5f, knob);
		r.drawCircle(pivot, 2.2f, Color{ 60, 62, 70, 255 });
	}

	void drawTimedSwitch(Renderer& r, Vector2 c, bool active, float timer, float window)
	{
		r.drawCircle(Vector2{ c.x + 1.5f, c.y + 2.0f }, 9.0f, Color{ 15, 30, 15, 90 });
		r.drawCircle(c, 9.0f, Color{ 70, 72, 80, 255 });
		r.drawCircleOutline(c, 9.0f, Color{ 35, 36, 42, 255 });

		if (active) {   // pressed: the ring drains as the time runs out
			r.drawCircle(c, 5.0f, Color{ 90, 235, 120, 255 });
			const float fraction = window > 0.0f ? std::clamp(timer / window, 0.0f, 1.0f) : 0.0f;
			if (fraction > 0.0f) arc(r, c, 12.0f, fraction, 3.0f, Color{ 120, 240, 150, 230 });
		}
		else {
			r.drawCircle(c, 6.5f, Color{ 235, 170, 50, 255 });
			r.drawCircle(Vector2{ c.x - 1.5f, c.y - 2.0f }, 2.0f, Color{ 255, 235, 170, 255 });
		}
	}

	void drawKey(Renderer& r, Vector2 c, float time)
	{
		const float bob = std::sin(time * 3.0f) * 1.5f;
		const Vector2 p{ c.x - 4.0f, c.y + bob };
		const Color gold{ 245, 200, 60, 255 };

		r.drawCircle(p, 11.0f, Color{ 255, 220, 90, 45 });
		r.drawCircle(p, 4.0f, gold);
		r.drawCircle(p, 1.8f, Color{ 70, 50, 20, 255 });                  // hole in the bow
		r.drawLine(Vector2{ p.x + 3.0f, p.y }, Vector2{ p.x + 11.0f, p.y }, 2.5f, gold);
		r.drawLine(Vector2{ p.x + 8.0f, p.y }, Vector2{ p.x + 8.0f, p.y + 3.5f }, 2.0f, gold);
		r.drawLine(Vector2{ p.x + 11.0f, p.y }, Vector2{ p.x + 11.0f, p.y + 3.0f }, 2.0f, gold);
	}

	void drawRune(Renderer& r, Vector2 c, const PuzzleItem& item, const PuzzleGate& gate, float time)
	{
		const bool lit = item.active;
		const bool wrong = gate.flash > 0.0f;

		r.drawRectangle(Rectangle{ c.x - 5.0f, c.y + 1.0f, 10.0f, 6.0f }, Color{ 95, 98, 106, 255 });
		r.drawRectangleOutline(Rectangle{ c.x - 5.0f, c.y + 1.0f, 10.0f, 6.0f }, 1.0f, Color{ 45, 46, 52, 255 });

		const Color color = wrong ? Color{ 255, 60, 60, 255 } : runeColor(item.index, lit ? 255 : 110);
		const Vector2 g{ c.x, c.y - 4.0f + (lit ? std::sin(time * 4.0f) : 0.0f) };

		if (lit || wrong) r.drawCircle(g, 9.0f, Color{ color.r, color.g, color.b, 60 });

		const float s = 5.5f;
		const Vector2 top{ g.x, g.y - s }, right{ g.x + s, g.y }, bottom{ g.x, g.y + s }, left{ g.x - s, g.y };
		r.drawLine(top, right, 2.0f, color);
		r.drawLine(right, bottom, 2.0f, color);
		r.drawLine(bottom, left, 2.0f, color);
		r.drawLine(left, top, 2.0f, color);
		r.drawCircle(g, 2.5f, color);
	}

	void drawTablet(Renderer& r, Vector2 c, bool read, float time)
	{
		const Rectangle slab{ c.x - 7.0f, c.y - 9.0f, 14.0f, 16.0f };
		r.drawRectangle(Rectangle{ slab.x + 1.5f, slab.y + 2.0f, slab.width, slab.height }, Color{ 15, 30, 15, 90 });
		r.drawRectangle(slab, Color{ 110, 114, 122, 255 });
		r.drawRectangleOutline(slab, 1.0f, Color{ 45, 46, 52, 255 });

		const Color glyph = read ? Color{ 255, 220, 100, 255 } : Color{ 60, 62, 70, 255 };
		for (int i = 0; i < 3; ++i) {
			const float y = slab.y + 4.0f + i * 4.0f;
			r.drawLine(Vector2{ slab.x + 3.0f, y }, Vector2{ slab.x + 11.0f - (i == 1 ? 3.0f : 0.0f), y }, 1.5f, glyph);
		}
		if (!read) r.drawCircle(Vector2{ c.x, c.y - 12.0f + std::sin(time * 3.0f) }, 2.0f, Color{ 255, 230, 120, 255 });   // "read me" marker
	}

	void drawGateEmblem(Renderer& r, const PuzzleGate& gate)
	{
		const Vector2 c{ gate.bounds.x + gate.bounds.width * 0.5f, gate.bounds.y + gate.bounds.height * 0.5f };
		const Color gold{ 245, 200, 60, 255 };

		r.drawCircle(c, 11.0f, Color{ 40, 40, 46, 255 });
		r.drawCircleOutline(c, 11.0f, Color{ 150, 150, 160, 255 });

		switch (gate.kind) {
		case PuzzleKind::Lever:
			r.drawLine(Vector2{ c.x - 4.0f, c.y + 5.0f }, Vector2{ c.x + 3.0f, c.y - 5.0f }, 2.5f, LIGHTGRAY);
			r.drawCircle(Vector2{ c.x + 3.0f, c.y - 5.0f }, 2.5f, Color{ 225, 70, 60, 255 });
			break;
		case PuzzleKind::Key:
			r.drawCircle(Vector2{ c.x - 3.5f, c.y }, 3.5f, gold);
			r.drawLine(Vector2{ c.x, c.y }, Vector2{ c.x + 6.0f, c.y }, 2.0f, gold);
			r.drawLine(Vector2{ c.x + 5.0f, c.y }, Vector2{ c.x + 5.0f, c.y + 3.0f }, 2.0f, gold);
			break;
		case PuzzleKind::TimedSwitch:
			r.drawCircleOutline(c, 6.5f, WHITE);
			r.drawLine(c, Vector2{ c.x, c.y - 4.5f }, 1.5f, WHITE);
			r.drawLine(c, Vector2{ c.x + 3.5f, c.y + 1.0f }, 1.5f, WHITE);
			break;
		case PuzzleKind::RuneSequence: {
			const int n = static_cast<int>(gate.order.size());
			for (int j = 0; j < n; ++j) {
				r.drawCircle(Vector2{ c.x + (static_cast<float>(j) - (n - 1) * 0.5f) * 5.0f, c.y }, 2.2f, runeColor(j));
			}
			break;
		}
		case PuzzleKind::KillGate:
			r.drawCircle(Vector2{ c.x, c.y - 1.0f }, 6.0f, Color{ 230, 230, 225, 255 });
			r.drawRectangle(Rectangle{ c.x - 3.5f, c.y + 3.0f, 7.0f, 4.0f }, Color{ 230, 230, 225, 255 });
			r.drawCircle(Vector2{ c.x - 2.5f, c.y - 1.5f }, 1.7f, Color{ 30, 30, 35, 255 });
			r.drawCircle(Vector2{ c.x + 2.5f, c.y - 1.5f }, 1.7f, Color{ 30, 30, 35, 255 });
			break;
		}
	}
}

void PuzzleRenderer::draw(Renderer& renderer, const std::vector<PuzzleItem>& items, const std::vector<PuzzleGate>& gates,
	float cellSize, const Vector2& viewCenter, float viewRadius)
{
	const float time = static_cast<float>(GetTime());
	const float reach = viewRadius + 24.0f;

	for (const PuzzleGate& gate : gates) {
		if (gate.open) continue;
		const Vector2 c{ gate.bounds.x + gate.bounds.width * 0.5f, gate.bounds.y + gate.bounds.height * 0.5f };
		if (inside(c, viewCenter, reach + gate.bounds.height * 0.5f)) drawGateEmblem(renderer, gate);
	}

	for (const PuzzleItem& item : items) {
		const Vector2 c{ item.cellX * cellSize + cellSize * 0.5f, item.cellY * cellSize + cellSize * 0.5f };
		if (!inside(c, viewCenter, reach)) continue;

		const PuzzleGate& gate = gates[item.gate];
		switch (item.kind) {
		case ItemKind::Lever:       drawLever(renderer, c, cellSize, item.active); break;
		case ItemKind::TimedSwitch: drawTimedSwitch(renderer, c, item.active, item.timer, gate.window); break;
		case ItemKind::Key:         if (!item.active) drawKey(renderer, c, time); break;
		case ItemKind::Rune:        drawRune(renderer, c, item, gate, time); break;
		case ItemKind::Tablet:      drawTablet(renderer, c, item.active, time); break;
		}
	}
}