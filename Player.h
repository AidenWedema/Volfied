#pragma once
#include "Object.h"
class Player : public Object
{
public:
	Player() : Object("Player") {};
	Player(Vector2 position) : Object("Player", position) {};
	~Player() {};
};

