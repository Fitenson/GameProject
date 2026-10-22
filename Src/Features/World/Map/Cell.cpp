#include "Cell.h"

Cell::Cell(const Vector2& position, const TileType& type): position(position), type(type) {}

const Vector2& Cell::getPosition() const { return position; }

const TileType& Cell::getType() const { return type; }