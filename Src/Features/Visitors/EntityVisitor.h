#pragma once
#include "Engine/DataStructure.h"

class Player;
class Zombie;
class Ally;

class EntityVisitor : public Engine::DataStructure::TreeVisitor {
public:
	virtual void visit(Player& player);
	virtual void visit(Zombie& zombie);
	virtual void visit(Ally& ally);
};