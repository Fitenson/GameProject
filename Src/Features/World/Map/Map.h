#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <raylib.h>
#include "Chunk.h"
#include "TileType.h"
#include "../Puzzle/Puzzle.h"
#include "../Map/StageConfig.h"

class Map {
public:
	static constexpr float CageReach = 40.0f;

	explicit Map(const StageConfig& config);

	const std::string& getName() const;
	std::vector<Chunk>& getExploredChunks();
	const std::vector<Chunk>& getExploredChunks() const;

	void loadChunks(const Vector2& center, float radius);
	bool isChunkExplored(int chunkX, int chunkY) const;

	TileType getTile(int cellX, int cellY) const;     // outside the map counts as Tree
	bool isBlocked(const Vector2& worldPosition) const;
	const Vector2& getSpawn() const;
	const std::vector<Vector2>& getWaypoints() const;
	Rectangle getPixelBounds() const;
	int getCellsX() const;
	int getCellsY() const;

	// ---- Puzzles
	void update(float deltaTime);                     // timed switches, wrong-rune flash
	void notifyKills(int totalKills);                 // opens kill gates
	bool interact(const Vector2& worldPosition);      // E: item, then locked door
	std::string getPrompt(const Vector2& worldPosition) const;   // "" when nothing is near
	std::string getObjective() const;
	bool allGatesOpen() const;
	const std::vector<PuzzleItem>& getItems() const;
	const std::vector<PuzzleGate>& getGates() const;
	int getKeysHeld() const;

	// ---- Prisoner
	bool isNearCage(const Vector2& worldPosition) const;
	bool freePrisoner(const Vector2& worldPosition);
	bool isPrisonerFreed() const;
	const Vector2& getPrisonerPosition() const;
	Rectangle getCageBounds() const;

private:
	std::string name;
	int cellsX = 0, cellsY = 0, chunksX = 0, chunksY = 0;

	std::vector<Chunk> exploredChunks;
	std::vector<TileType> tiles;
	std::vector<Vector2> waypoints;
	std::vector<PuzzleItem> items;
	std::vector<PuzzleGate> gates;
	Vector2 spawn{ 0.0f, 0.0f };

	int cageX = 0, cageY = 0;
	bool prisonerFreed = false;
	Vector2 prisoner{ 0.0f, 0.0f };

	int keysHeld = 0;
	int totalKills = 0;

	void setTile(int cellX, int cellY, TileType type);
	void openGate(size_t index);
	int findItem(const Vector2& worldPosition) const;
	void use(PuzzleItem& item);
	bool allActive(size_t gate, ItemKind kind) const;
	void loadChunk(int chunkX, int chunkY);
	void unloadDistantChunks(const Vector2& center, float radius);
	static bool circleTouchesChunk(const Vector2& center, float radius, int chunkX, int chunkY);
};