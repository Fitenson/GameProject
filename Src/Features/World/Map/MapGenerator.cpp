// MapGenerator.cpp
#include <algorithm>
#include <cmath>
#include <numeric>
#include <utility>
#include "MapGenerator.h"
#include "Cell.h"
#include "Engine/Core.h"

namespace {
	constexpr float Pi = 3.14159265f;
	constexpr int BorderThickness = 3;
	constexpr float PlazaRadius = 8.0f;
	constexpr float PlazaSpacing = PlazaRadius * 2.0f + 4.0f;
	constexpr float RegionPad = PlazaRadius + 4.0f;     // keeps clearings off the walls
	constexpr float EdgeMargin = 12.0f;
	constexpr float SpawnX = 12.0f;
	constexpr float ClearingRadius = 8.0f;
	constexpr float GateFlatten = 10.0f;                // road is flat this close to a gate
	constexpr float RuneSpacing = 4.0f;

	struct Road {
		std::vector<Vector2> points;                    // centreline, in cells
		float halfWidth = 0.0f;
		float minX = 0, maxX = 0, minY = 0, maxY = 0;   // reach, for a quick reject
	};

	Road makeRoad(std::vector<Vector2> points, float halfWidth)
	{
		Road road;
		road.points = std::move(points);
		road.halfWidth = halfWidth;
		road.minX = road.maxX = road.points.front().x;
		road.minY = road.maxY = road.points.front().y;
		for (const Vector2& p : road.points) {
			road.minX = std::min(road.minX, p.x); road.maxX = std::max(road.maxX, p.x);
			road.minY = std::min(road.minY, p.y); road.maxY = std::max(road.maxY, p.y);
		}
		const float reach = halfWidth + 8.0f;   // forest density only looks 8 cells past the edge
		road.minX -= reach; road.maxX += reach; road.minY -= reach; road.maxY += reach;
		return road;
	}

	float distanceToSegment(float px, float py, Vector2 a, Vector2 b)
	{
		const float abx = b.x - a.x, aby = b.y - a.y;
		const float lengthSquared = abx * abx + aby * aby;
		float t = lengthSquared > 0.0f ? ((px - a.x) * abx + (py - a.y) * aby) / lengthSquared : 0.0f;
		t = std::clamp(t, 0.0f, 1.0f);
		const float dx = px - (a.x + abx * t);
		const float dy = py - (a.y + aby * t);
		return std::sqrt(dx * dx + dy * dy);
	}

	// Distance to the road centreline, or 1e9 when the cell is far from the road.
	float distanceToRoad(float x, float y, const Road& road)
	{
		if (x < road.minX || x > road.maxX || y < road.minY || y > road.maxY) return 1e9f;
		float best = 1e9f;
		for (size_t i = 0; i + 1 < road.points.size(); ++i) {
			best = std::min(best, distanceToSegment(x, y, road.points[i], road.points[i + 1]));
		}
		return best;
	}

	float smoothstep(float t)
	{
		t = std::clamp(t, 0.0f, 1.0f);
		return t * t * (3.0f - 2.0f * t);
	}

	// y(x) = centre + A1 sin(f1 x + p1) + A2 sin(f2 x + p2), flattened near every gate.
	struct MainCurve {
		float centerY = 0, a1 = 0, f1 = 0, p1 = 0, a2 = 0, f2 = 0, p2 = 0;
		std::vector<float> gateX;

		float raw(float x) const { return centerY + a1 * std::sin(f1 * x + p1) + a2 * std::sin(f2 * x + p2); }

		float at(float x) const {
			float y = raw(x);
			for (float gx : gateX) {
				const float w = smoothstep(std::abs(x - gx) / GateFlatten);   // 0 at the gate, 1 far away
				y = raw(gx) * (1.0f - w) + y * w;
			}
			return y;
		}
	};

	struct Region { float lo, hi; };

	// A curvy path from a to b: a bulge plus a smaller wobble, both vanishing at the ends.
	std::vector<Vector2> makeTrail(Vector2 a, Vector2 b, float bulge, float wobble, int waves, int steps,
		float xLo, float xHi, float yLo, float yHi)
	{
		const Vector2 d{ b.x - a.x, b.y - a.y };
		const float length = std::max(1.0f, std::sqrt(d.x * d.x + d.y * d.y));
		const Vector2 normal{ -d.y / length, d.x / length };

		std::vector<Vector2> points;
		for (int i = 0; i <= steps; ++i) {
			const float t = static_cast<float>(i) / static_cast<float>(steps);
			const float offset = bulge * std::sin(Pi * t) + wobble * std::sin(2.0f * Pi * waves * t) * std::sin(Pi * t);
			Vector2 p{ a.x + d.x * t + normal.x * offset, a.y + d.y * t + normal.y * offset };
			p.x = std::clamp(p.x, xLo, xHi);
			p.y = std::clamp(p.y, yLo, yHi);
			points.push_back(p);
		}
		return points;
	}
}

MapLayout MapGenerator::generate(const StageConfig& config)
{
	SeededRandom rng(config.seed);
	MapLayout layout;

	const int W = config.mapCells;
	const int H = config.mapCells;
	const float fw = static_cast<float>(W);
	const float fh = static_cast<float>(H);
	const float cell = static_cast<float>(Cell::Size);
	const int gateCount = static_cast<int>(config.puzzles.size());
	const auto toPixels = [cell](Vector2 c) { return Vector2{ c.x * cell + cell * 0.5f, c.y * cell + cell * 0.5f }; };

	layout.cellsX = W;
	layout.cellsY = H;

	// ---- Gate columns, evenly spaced
	std::vector<float> gateX;
	for (int i = 0; i < gateCount; ++i) {
		gateX.push_back(std::round(fw * static_cast<float>(i + 1) / static_cast<float>(gateCount + 1)));
	}

	// ---- Main road: two sine waves whose size grows with the stage
	MainCurve curve;
	curve.centerY = fh * 0.5f;
	const float maxAmp = fh * 0.5f - 16.0f - config.mainRoadHalfWidth;
	curve.a1 = std::min(maxAmp * 0.6f, 4.0f + 1.6f * config.complexity);
	curve.f1 = rng.real(0.035f, 0.055f);
	curve.p1 = rng.real(0.0f, 2.0f * Pi);
	curve.a2 = std::min(maxAmp * 0.3f, 1.0f + 0.8f * config.complexity);
	curve.f2 = rng.real(0.08f, 0.12f) + 0.008f * config.complexity;
	curve.p2 = rng.real(0.0f, 2.0f * Pi);
	curve.gateX = gateX;

	std::vector<float> xs;
	for (float x = SpawnX; x <= fw - SpawnX; x += 6.0f) xs.push_back(x);
	xs.insert(xs.end(), gateX.begin(), gateX.end());
	std::sort(xs.begin(), xs.end());

	std::vector<Road> roads;
	std::vector<Vector2> mainPoints;
	for (float x : xs) {
		mainPoints.push_back(Vector2{ x, curve.at(x) });
		layout.waypoints.push_back(toPixels(mainPoints.back()));
	}
	roads.push_back(makeRoad(mainPoints, config.mainRoadHalfWidth));

	const Vector2 spawnCell{ SpawnX, curve.at(SpawnX) };
	layout.spawn = toPixels(spawnCell);

	// ---- Regions: the stretch of road before each gate, plus the stretch after the last one
	std::vector<Region> regions;
	for (int r = 0; r <= gateCount; ++r) {
		const float lo = (r == 0) ? SpawnX + 10.0f : gateX[static_cast<size_t>(r - 1)] + RegionPad + 1.0f;
		float hi = (r < gateCount) ? gateX[static_cast<size_t>(r)] - RegionPad - 1.0f : fw - 14.0f;
		if (hi < lo + 4.0f) hi = lo + 4.0f;
		regions.push_back(Region{ lo, hi });
	}

	// ---- Trails and clearings
	std::vector<Vector2> plazas;
	const float yLo = EdgeMargin, yHi = fh - EdgeMargin;
	const int steps = 6 + config.complexity / 2;
	const float wobble = std::min(8.0f, 1.5f + 0.6f * config.complexity);

	const auto addTrail = [&](Vector2 from, Vector2 to, float bulge, const Region& region) {
		roads.push_back(makeRoad(
			makeTrail(from, to, bulge, wobble, config.wobbleWaves, steps, region.lo - 3.0f, region.hi + 3.0f, yLo, yHi),
			config.trailHalfWidth));
		};

	// Finds a clearing off the main road inside a region, links it with a trail, returns its centre.
	const auto addSpot = [&](const Region& region) -> Vector2 {
		Vector2 anchor{ region.lo, curve.at(region.lo) };
		Vector2 plaza{ region.lo, std::clamp(anchor.y + 20.0f, yLo, yHi) };

		for (int attempt = 0; attempt < 40; ++attempt) {
			const float ax = rng.real(region.lo, region.hi);
			const float ay = curve.at(ax);
			const float dir = rng.chance(0.5f) ? 1.0f : -1.0f;
			const float room = dir > 0.0f ? yHi - ay : ay - yLo;
			if (room < 20.0f) continue;

			anchor = Vector2{ ax, ay };
			plaza = Vector2{ std::clamp(ax + rng.real(-8.0f, 8.0f), region.lo, region.hi),
							 ay + dir * rng.real(18.0f, room) };

			const bool apart = std::all_of(plazas.begin(), plazas.end(), [&](const Vector2& other) {
				return std::hypot(other.x - plaza.x, other.y - plaza.y) >= PlazaSpacing;
				});
			if (apart) break;
		}

		addTrail(anchor, plaza, rng.real(-6.0f, 6.0f), region);
		plazas.push_back(plaza);
		return plaza;
		};

	// Loops: two points on the main road joined by a long bulging path.
	for (int i = 0; i < config.loops; ++i) {
		const Region& region = regions[static_cast<size_t>(rng.integer(0, gateCount))];
		if (region.hi - region.lo < 24.0f) continue;

		const float x1 = rng.real(region.lo, region.hi - 20.0f);
		const float x2 = x1 + rng.real(14.0f, std::min(30.0f, region.hi - x1));
		const float bulge = rng.real(14.0f, 26.0f) * (rng.chance(0.5f) ? 1.0f : -1.0f);
		addTrail(Vector2{ x1, curve.at(x1) }, Vector2{ x2, curve.at(x2) }, bulge, region);
	}

	// Dead-end trails that hold nothing (exploration and distraction).
	for (int i = 0; i < config.decoyTrails; ++i) {
		addSpot(regions[static_cast<size_t>(rng.integer(0, gateCount))]);
	}

	// ---- Puzzles: items go in the region before their gate
	for (int g = 0; g < gateCount; ++g) {
		PuzzleGate gate;
		gate.kind = config.puzzles[static_cast<size_t>(g)];
		gate.column = static_cast<int>(gateX[static_cast<size_t>(g)]);
		const Region& region = regions[static_cast<size_t>(g)];

		const auto addItem = [&](ItemKind kind, Vector2 p, int index = 0) {
			PuzzleItem item;
			item.kind = kind;
			item.cellX = static_cast<int>(std::round(p.x));
			item.cellY = static_cast<int>(std::round(p.y));
			item.gate = static_cast<size_t>(g);
			item.index = index;
			layout.items.push_back(item);
			};

		switch (gate.kind) {
		case PuzzleKind::Lever:
			for (int k = 0; k < config.leversPerGate; ++k) addItem(ItemKind::Lever, addSpot(region));
			break;

		case PuzzleKind::Key:
			gate.keysNeeded = config.keysPerGate;
			for (int k = 0; k < config.keysPerGate; ++k) addItem(ItemKind::Key, addSpot(region));
			break;

		case PuzzleKind::TimedSwitch:
			gate.window = config.timedWindow;
			for (int k = 0; k < config.timedSwitches; ++k) addItem(ItemKind::TimedSwitch, addSpot(region));
			break;

		case PuzzleKind::RuneSequence: {
			const int count = config.runeCount;
			gate.order.resize(static_cast<size_t>(count));
			std::iota(gate.order.begin(), gate.order.end(), 0);
			for (int i = count - 1; i > 0; --i) {
				std::swap(gate.order[static_cast<size_t>(i)], gate.order[static_cast<size_t>(rng.integer(0, i))]);
			}

			const Vector2 centre = addSpot(region);                       // runes side by side
			for (int j = 0; j < count; ++j) {
				addItem(ItemKind::Rune, Vector2{ centre.x + (static_cast<float>(j) - (count - 1) * 0.5f) * RuneSpacing, centre.y }, j);
			}
			addItem(ItemKind::Tablet, addSpot(region));                   // the hint, somewhere else
			break;
		}

		case PuzzleKind::KillGate:
			gate.killsNeeded = std::max(1, config.killTarget * (g + 1) / (gateCount + 2));
			break;
		}

		layout.gates.push_back(std::move(gate));
	}

	// ---- Tiles
	layout.tiles.assign(static_cast<size_t>(W) * H, TileType::Grass);

	for (int y = 0; y < H; ++y) {
		for (int x = 0; x < W; ++x) {
			const float fx = static_cast<float>(x), fy = static_cast<float>(y);

			float outside = 1e9f;   // distance past the road edge (<= 0 means on the road)
			for (const Road& road : roads) {
				const float d = distanceToRoad(fx, fy, road);
				if (d < 1e9f) outside = std::min(outside, d - road.halfWidth);
			}

			bool inPlaza = false;
			for (const Vector2& p : plazas) {
				if (std::hypot(fx - p.x, fy - p.y) <= PlazaRadius) { inPlaza = true; break; }
			}

			int gateIndex = -1;
			for (int g = 0; g < gateCount; ++g) {
				if (std::abs(x - static_cast<int>(gateX[static_cast<size_t>(g)])) <= 1) gateIndex = g;
			}

			const int edge = std::min({ x, y, W - 1 - x, H - 1 - y });
			const float spawnDistance = std::hypot(fx - spawnCell.x, fy - spawnCell.y);
			const uint32_t hash = hashCell(x, y);

			TileType type = TileType::Grass;

			if (edge < BorderThickness) {
				type = TileType::Tree;
			}
			else if (gateIndex >= 0) {
				if (outside <= 0.0f) {
					type = TileType::Gate;
					PuzzleGate& gate = layout.gates[static_cast<size_t>(gateIndex)];
					gate.x0 = std::min(gate.x0, x); gate.x1 = std::max(gate.x1, x);
					gate.y0 = std::min(gate.y0, y); gate.y1 = std::max(gate.y1, y);
				}
				else {
					type = TileType::Wall;
				}
			}
			else if (outside <= 0.0f || inPlaza) {
				type = TileType::Path;
			}
			else if (spawnDistance < ClearingRadius) {
				type = TileType::Grass;
			}
			else {
				const uint32_t density = outside < 3.0f ? 4 : (outside < 7.0f ? 18 : 45);
				if ((hash >> 16) % 100 < density) {
					type = ((hash >> 4) % 100 < 15) ? TileType::Rock : TileType::Tree;
				}
			}

			layout.tiles[static_cast<size_t>(y) * W + x] = type;
		}
	}

	// ---- Gate bounds in pixels
	for (PuzzleGate& gate : layout.gates) {
		if (gate.x1 < gate.x0) {   // no road crossed this wall (should not happen)
			gate.x0 = gate.x1 = gate.column;
			gate.y0 = gate.y1 = static_cast<int>(std::round(curve.at(static_cast<float>(gate.column))));
		}
		gate.bounds = Rectangle{ gate.x0 * cell, gate.y0 * cell,
								 (gate.x1 - gate.x0 + 1) * cell, (gate.y1 - gate.y0 + 1) * cell };
	}

	// ---- Prison cage in the last region, on the main road
	const float cageXf = (gateX.back() + fw - 14.0f) * 0.5f;
	layout.cageX = static_cast<int>(std::round(cageXf));
	layout.cageY = static_cast<int>(std::round(curve.at(cageXf)));
	for (int y = layout.cageY - 1; y <= layout.cageY + 1; ++y) {
		for (int x = layout.cageX - 1; x <= layout.cageX + 1; ++x) {
			if (x >= 0 && y >= 0 && x < W && y < H) layout.tiles[static_cast<size_t>(y) * W + x] = TileType::Cage;
		}
	}

	return layout;
}