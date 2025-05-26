#pragma once
#include <string>
#include <SFML/Graphics.hpp>

class Player;
class Powerup
{
public:
	~Powerup() {};

	virtual void Start() {};
	virtual void Use() {};
	virtual void End() {};
	virtual void Upgrade() { Start(); };
	virtual void Draw(sf::RenderTarget& target) {};

	bool operator==(const Powerup& other) const {
		return name == other.name;
	}

	std::string name;
	float timer = 0;
	Player* user;
};