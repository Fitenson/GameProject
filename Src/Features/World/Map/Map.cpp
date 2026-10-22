#include <algorithm>
#include <cmath>
#include <utility>
#include "Map.h"
#include "Cell.h"
#include "MapGenerator.h"

namespace {
	constexpr float ItemReach = 22.0f;   // pixels
	constexpr float DoorReach = 30.0f;

	float distanceToRect(const Vector2& p, const Rectangle& r)
	{
		const float dx = std::max({ r.x - p.x, 0.0f, p.x - (r.x + r.width) });
		const float dy = std::max({ r.y - p.y, 0.0f, p.y - (r.y + r.height) });
		return std::sqrt(dx * dx + dy * dy);
	}
}

Map::Map(const StageConfig& config) : name("Stage " + std::to_string(config.number))
{
	MapLayout layout = MapGenerator::generate(config);

	cellsX = layout.cellsX;
	cellsY = layout.cellsY;
	chunksX = cellsX / static_cast<int>(Chunk::Width);
	chunksY = cellsY / static_cast<int>(Chunk::Height);

	tiles = std::move(layout.tiles);
	waypoints = std::move(layout.waypoints);
	items = std::move(layout.items);
	gates = std::move(layout.gates);
	spawn = layout.spawn;
	cageX = layout.cageX;
	cageY = layout.cageY;

	const float cell = static_cast<float>(Cell::Size);
	prisoner = Vector2{ cageX * cell + cell * 0.5f, cageY * cell + cell * 0.5f };

	exploredChunks.reserve(64);
}

// ---------- Tiles ----------

const std::string& Map::getName() const { return name; }
int Map::getCellsX() const { return cellsX; }
int Map::getCellsY() const { return cellsY; }

TileType Map::getTile(int cellX, int cellY) const
{
	if (cellX < 0 || cellY < 0 || cellX >= cellsX || cellY >= cellsY) return TileType::Tree;
	return tiles[static_cast<size_t>(cellY) * cellsX + cellX];
}

void Map::setTile(int cellX, int cellY, TileType type)
{
	if (cellX < 0 || cellY < 0 || cellX >= cellsX || cellY >= cellsY) return;
	tiles[static_cast<size_t>(cellY) * cellsX + cellX] = type;
}

bool Map::isBlocked(const Vector2& worldPosition) const
{
	const float cell = static_cast<float>(Cell::Size);
	return isBlocking(getTile(
		static_cast<int>(std::floor(worldPosition.x / cell)),
		static_cast<int>(std::floor(worldPosition.y / cell))));
}

const Vector2& Map::getSpawn() const { return spawn; }
const std::vector<Vector2>& Map::getWaypoints() const { return waypoints; }

Rectangle Map::getPixelBounds() const
{
	const float cell = static_cast<float>(Cell::Size);
	return Rectangle{ 0.0f, 0.0f, cellsX * cell, cellsY * cell };
}

// ---------- Chunks ----------

std::vector<Chunk>& Map::getExploredChunks() { return exploredChunks; }
const std::vector<Chunk>& Map::getExploredChunks() const { return exploredChunks; }

bool Map::isChunkExplored(int chunkX, int chunkY) const
{
	for (const Chunk& chunk : exploredChunks) {
		const Vector2& p = chunk.getPosition();
		if (static_cast<int>(p.x) == chunkX && static_cast<int>(p.y) == chunkY) return true;
	}
	return false;
}

void Map::loadChunks(const Vector2& center, float radius)
{
	const float chunkW = static_cast<float>(Chunk::Width * Cell::Size);
	const float chunkH = static_cast<float>(Chunk::Height * Cell::Size);

	const int minX = std::max(0, static_cast<int>(std::floor((center.x - radius) / chunkW)));
	const int maxX = std::min(chunksX - 1, static_cast<int>(std::floor((center.x + radius) / chunkW)));
	const int minY = std::max(0, static_cast<int>(std::floor((center.y - radius) / chunkH)));
	const int maxY = std::min(chunksY - 1, static_cast<int>(std::floor((center.y + radius) / chunkH)));

	for (int y = minY; y <= maxY; ++y) {
		for (int x = minX; x <= maxX; ++x) {
			if (circleTouchesChunk(center, radius, x, y)) loadChunk(x, y);
		}
	}

	unloadDistantChunks(center, radius);
}

void Map::loadChunk(int chunkX, int chunkY)
{
	if (isChunkExplored(chunkX, chunkY)) return;

	std::vector<TileType> chunkTiles;
	chunkTiles.reserve(Chunk::Width * Chunk::Height);

	for (int y = 0; y < static_cast<int>(Chunk::Height); ++y) {
		for (int x = 0; x < static_cast<int>(Chunk::Width); ++x) {
			chunkTiles.push_back(getTile(chunkX * static_cast<int>(Chunk::Width) + x,
				chunkY * static_cast<int>(Chunk::Height) + y));
		}
	}

	exploredChunks.emplace_back(Vector2{ static_cast<float>(chunkX), static_cast<float>(chunkY) }, chunkTiles);
}

void Map::unloadDistantChunks(const Vector2& center, float radius)
{
	std::erase_if(exploredChunks, [&](const Chunk& chunk) {
		const Vector2& p = chunk.getPosition();
		return !circleTouchesChunk(center, radius, static_cast<int>(p.x), static_cast<int>(p.y));
		});
}

bool Map::circleTouchesChunk(const Vector2& center, float radius, int chunkX, int chunkY)
{
	const float w = static_cast<float>(Chunk::Width * Cell::Size);
	const float h = static_cast<float>(Chunk::Height * Cell::Size);
	const float left = chunkX * w;
	const float top = chunkY * h;

	const float nearestX = std::clamp(center.x, left, left + w);
	const float nearestY = std::clamp(center.y, top, top + h);
	const float dx = center.x - nearestX;
	const float dy = center.y - nearestY;
	return dx * dx + dy * dy <= radius * radius;
}

// ---------- Puzzles ----------

const std::vector<PuzzleItem>& Map::getItems() const { return items; }
const std::vector<PuzzleGate>& Map::getGates() const { return gates; }
int Map::getKeysHeld() const { return keysHeld; }

bool Map::allGatesOpen() const
{
	return std::all_of(gates.begin(), gates.end(), [](const PuzzleGate& gate) { return gate.open; });
}

bool Map::allActive(size_t gate, ItemKind kind) const
{
	bool any = false;
	for (const PuzzleItem& item : items) {
		if (item.gate != gate || item.kind != kind) continue;
		any = true;
		if (!item.active) return false;
	}
	return any;
}

void Map::update(float deltaTime)
{
	for (PuzzleGate& gate : gates) gate.flash = std::max(0.0f, gate.flash - deltaTime);

	for (PuzzleItem& item : items) {
		if (item.kind != ItemKind::TimedSwitch || !item.active || gates[item.gate].open) continue;
		item.timer -= deltaTime;
		if (item.timer <= 0.0f) { item.active = false; item.timer = 0.0f; }
	}
}

void Map::notifyKills(int kills)
{
	totalKills = kills;
	for (size_t g = 0; g < gates.size(); ++g) {
		if (gates[g].kind == PuzzleKind::KillGate && !gates[g].open && kills >= gates[g].killsNeeded) openGate(g);
	}
}

void Map::openGate(size_t index)
{
	PuzzleGate& gate = gates[index];
	gate.open = true;

	for (int y = gate.y0; y <= gate.y1; ++y) {
		for (int x = gate.x0; x <= gate.x1; ++x) {
			if (getTile(x, y) == TileType::Gate) setTile(x, y, TileType::Path);
		}
	}
	exploredChunks.clear();   // chunks copied their tiles, so they reload with the open door
}

// The nearest item in reach that can still be used, or -1.
int Map::findItem(const Vector2& p) const
{
	const float cell = static_cast<float>(Cell::Size);
	int best = -1;
	float bestDistance = ItemReach * ItemReach;

	for (size_t i = 0; i < items.size(); ++i) {
		const PuzzleItem& item = items[i];
		if (gates[item.gate].open) continue;
		if (item.active && item.kind != ItemKind::Tablet) continue;

		const float dx = p.x - (item.cellX * cell + cell * 0.5f);
		const float dy = p.y - (item.cellY * cell + cell * 0.5f);
		const float d = dx * dx + dy * dy;
		if (d <= bestDistance) { bestDistance = d; best = static_cast<int>(i); }
	}
	return best;
}

void Map::use(PuzzleItem& item)
{
	PuzzleGate& gate = gates[item.gate];

	switch (item.kind) {
	case ItemKind::Lever:
		item.active = true;
		if (allActive(item.gate, ItemKind::Lever)) openGate(item.gate);
		break;

	case ItemKind::Key:
		item.active = true;
		++keysHeld;
		break;

	case ItemKind::TimedSwitch:
		item.active = true;
		item.timer = gate.window;
		if (allActive(item.gate, ItemKind::TimedSwitch)) openGate(item.gate);   // all pressed at once
		break;

	case ItemKind::Rune:
		if (item.index == gate.order[gate.progress]) {
			item.active = true;
			if (++gate.progress == gate.order.size()) openGate(item.gate);
		}
		else {   // wrong rune: everything resets
			gate.progress = 0;
			gate.flash = 0.6f;
			for (PuzzleItem& other : items) {
				if (other.gate == item.gate && other.kind == ItemKind::Rune) other.active = false;
			}
		}
		break;

	case ItemKind::Tablet:
		item.active = true;
		gate.orderKnown = true;
		break;
	}
}

bool Map::interact(const Vector2& p)
{
	const int index = findItem(p);
	if (index >= 0) {
		use(items[static_cast<size_t>(index)]);
		return true;
	}

	// A locked door opens with the keys the player carries.
	for (size_t g = 0; g < gates.size(); ++g) {
		PuzzleGate& gate = gates[g];
		if (gate.open || gate.kind != PuzzleKind::Key) continue;
		if (distanceToRect(p, gate.bounds) > DoorReach || keysHeld < gate.keysNeeded) continue;

		keysHeld -= gate.keysNeeded;
		openGate(g);
		return true;
	}
	return false;
}

std::string Map::getPrompt(const Vector2& p) const
{
	const int index = findItem(p);
	if (index >= 0) {
		switch (items[static_cast<size_t>(index)].kind) {
		case ItemKind::Lever:       return "Press E to pull the lever";
		case ItemKind::TimedSwitch: return "Press E to press the switch (it resets!)";
		case ItemKind::Key:         return "Press E to take the key";
		case ItemKind::Rune:        return "Press E to touch the rune";
		case ItemKind::Tablet:      return "Press E to read the tablet";
		}
	}

	for (const PuzzleGate& gate : gates) {
		if (gate.open || gate.kind != PuzzleKind::Key || distanceToRect(p, gate.bounds) > DoorReach) continue;
		return keysHeld >= gate.keysNeeded
			? "Press E to unlock the door"
			: "Locked: needs " + std::to_string(gate.keysNeeded) + " key(s)";
	}

	if (isNearCage(p)) return "Press E to free the prisoner";
	return "";
}

std::string Map::getObjective() const
{
	const auto progress = [&](size_t g, ItemKind kind) {
		int done = 0, total = 0;
		for (const PuzzleItem& item : items) {
			if (item.gate != g || item.kind != kind) continue;
			++total;
			if (item.active) ++done;
		}
		return std::to_string(done) + "/" + std::to_string(total);
		};

	for (size_t g = 0; g < gates.size(); ++g) {
		const PuzzleGate& gate = gates[g];
		if (gate.open) continue;

		const std::string label = "Gate " + std::to_string(g + 1) + ": ";
		switch (gate.kind) {
		case PuzzleKind::Lever:
			return label + "pull the levers (" + progress(g, ItemKind::Lever) + ")";
		case PuzzleKind::Key:
			return label + "find the key (" + std::to_string(keysHeld) + "/" +
				std::to_string(gate.keysNeeded) + "), then unlock the door";
		case PuzzleKind::TimedSwitch:
			return label + "press every switch within " + std::to_string(static_cast<int>(gate.window)) +
				"s (" + progress(g, ItemKind::TimedSwitch) + ")";
		case PuzzleKind::RuneSequence: {
			if (!gate.orderKnown) return label + "find the tablet that shows the rune order";
			std::string text = label + "touch the runes: ";
			for (size_t i = 0; i < gate.order.size(); ++i) {
				text += runeName(gate.order[i]);
				if (i + 1 < gate.order.size()) text += " > ";
			}
			return text;
		}
		case PuzzleKind::KillGate:
			return label + "defeat " + std::to_string(std::max(0, gate.killsNeeded - totalKills)) + " more enemies";
		}
	}

	return prisonerFreed ? "All gates are open. Survive!" : "All gates are open. Free the prisoner!";
}

// ---------- Prisoner ----------

bool Map::isNearCage(const Vector2& worldPosition) const
{
	if (prisonerFreed) return false;
	const float dx = worldPosition.x - prisoner.x;
	const float dy = worldPosition.y - prisoner.y;
	return dx * dx + dy * dy <= CageReach * CageReach;
}

bool Map::freePrisoner(const Vector2& worldPosition)
{
	if (!isNearCage(worldPosition)) return false;

	prisonerFreed = true;
	for (int y = cageY - 1; y <= cageY + 1; ++y) {
		for (int x = cageX - 1; x <= cageX + 1; ++x) setTile(x, y, TileType::Path);
	}
	exploredChunks.clear();
	return true;
}

bool Map::isPrisonerFreed() const { return prisonerFreed; }
const Vector2& Map::getPrisonerPosition() const { return prisoner; }

Rectangle Map::getCageBounds() const
{
	const float cell = static_cast<float>(Cell::Size);
	return Rectangle{ (cageX - 1) * cell, (cageY - 1) * cell, 3.0f * cell, 3.0f * cell };
}