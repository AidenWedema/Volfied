#include "Ship.h"
#include "Debug.hpp"

void Ship::Awake()
{
	if (!texture.loadFromFile("assets/sprites/ship.png")) {
		std::cerr << "Error loading ship texture\n";
		return;
	}
	sprite.setTexture(texture);
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
}

void Ship::Start()
{
	direction = RNG::GetRange(0.0f, 22.5f) * 16 + 11.25f;
}

void Ship::Update()
{
	position = position + Vector2::FromDegrees(direction) * speed;

	// Keep the enemy in bounds
	Vector2 pos = position;
	if (Playfield::GetInstance()->IsInBounds(position, sprite.getGlobalBounds(), true)) {
		Vector2 a = Vector2::FromDegrees(direction);
		Vector2 normal = Vector2::Normalize(pos - position);
		direction = Vector2::Degrees(Vector2::Reflect(a, normal));
	}
	Vector2 fieldSize = Playfield::GetInstance()->GetSize();
}
