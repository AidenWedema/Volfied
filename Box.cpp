#include "Box.h"
#include "Playfield.h"
#include "Score.hpp"
#include "Player.h"
#include "TimeFreeze.h"
#include "Gun.h"

void Box::Awake()
{
	if (!texture.loadFromFile("assets/sprites/box.png")) {
		std::cerr << "Error loading box texture\n";
		return;
	}
	sprite.setTexture(texture);
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
}

void Box::Start()
{
}

void Box::Update()
{
	if (inactive) return;

	Vector2 extents = Vector2(sprite.getLocalBounds().width, sprite.getLocalBounds().height) / 2;
	std::vector<Vector2> points = {
		Vector2(position.x - extents.x, position.y - extents.y),
		Vector2(position.x + extents.x, position.y - extents.y),
		Vector2(position.x + extents.x, position.y + extents.y),
		Vector2(position.x - extents.x, position.y + extents.y)
	};

	bool inWall = false;
	for (auto& point : points) {
		inWall = false;
		for (auto& area : *Playfield::GetInstance()->GetWallArea()) {
			if (area.Contains(point, true)) {
				inWall = true;
				break;
			}
		}
		if (!inWall) break;
	}
	if (inWall) {
		Destroy(this);
		Score::Add(1);
		// Get a random powerup from the box
		int randomIndex = RNG::GetRange(0, powers.size() - 1);
		std::string powerName = powers[randomIndex];
		Powerup* power = GetPower(powerName);
		if (power == nullptr) return;
		Player::GetActivePlayer()->AddPower(power);
	}
}

void Box::Draw(sf::RenderTarget& target)
{
	if (clipped) return;
	sprite.setPosition(position.x, position.y);
	target.draw(sprite);
}

Powerup* Box::GetPower(std::string name)
{
	if (name == "TimeFreeze") return new TimeFreeze();
	else if (name == "Gun") return new Gun();
	else return nullptr;
}
