#pragma once
#include <string>
class Player;
class Powerup
{
public:
	~Powerup() {};

	virtual void Start() {};
	virtual void Use() {};
	virtual void End() {};

	bool operator==(const Powerup& other) const {
		return name == other.name;
	}

	std::string name;
	float timer = 0;
	Player* user;
};