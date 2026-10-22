#pragma once

#include <cstddef>
#include <vector>
#include <raylib.h>

enum class PuzzleKind { Lever, Key, TimedSwitch, RuneSequence, KillGate };
enum class ItemKind { Lever, TimedSwitch, Key, Rune, Tablet };

inline const char* puzzleName(PuzzleKind kind)
{
	switch (kind) {
	case PuzzleKind::Lever:        return "Lever";
	case PuzzleKind::Key:          return "Key door";
	case PuzzleKind::TimedSwitch:  return "Timed switch";
	case PuzzleKind::RuneSequence: return "Rune puzzle";
	case PuzzleKind::KillGate:     return "Kill gate";
	}
	return "";
}

inline const char* runeName(int index)
{
	static const char* names[4] = { "Red", "Blue", "Gold", "Violet" };
	return names[((index % 4) + 4) % 4];
}

// Something the player can use. Its position is in cells.
struct PuzzleItem {
	ItemKind kind = ItemKind::Lever;
	int cellX = 0;
	int cellY = 0;
	size_t gate = 0;          // the gate this item belongs to
	int index = 0;            // rune: its colour
	bool active = false;      // lever pulled, key taken, rune lit, tablet read
	float timer = 0.0f;       // timed switch: seconds left while pressed
};

struct PuzzleGate {
	PuzzleKind kind = PuzzleKind::Lever;
	int column = 0;                                   // centre column of the wall
	int x0 = 1 << 30, y0 = 1 << 30, x1 = -1, y1 = -1; // cells covered by the door tiles
	Rectangle bounds{};                               // the same area in pixels
	bool open = false;

	int killsNeeded = 0;           // KillGate
	int keysNeeded = 0;            // Key
	float window = 0.0f;           // TimedSwitch: seconds the switches stay pressed
	std::vector<int> order;        // RuneSequence: correct order of rune colours
	size_t progress = 0;
	bool orderKnown = false;       // the tablet was read
	float flash = 0.0f;            // wrong-rune feedback
};