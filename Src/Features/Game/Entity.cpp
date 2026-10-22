#include "Entity.h"

Entity::Entity(const Vector2& position) : position(position) {}

Vector2 Entity::getPosition() const { return position; }
void Entity::setPosition(Vector2& position) { this->position = position; }
void Entity::setPosition(const Vector2& position) { this->position = position; }