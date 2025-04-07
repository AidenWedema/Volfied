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
	Vector2 fieldSize = Playfield::GetInstance()->GetSize();
	if (position.x <= 0) {
		Vector2 point = Vector2(0, position.y);
		Vector2 a = Vector2::FromDegrees(direction);
		direction = Vector2::Degrees(Vector2::Reflect(a, Vector2::Left()));
		position = point;
	}
	if (position.y <= 0) {
		Vector2 point = Vector2(position.x, 0);
		Vector2 a = Vector2::FromDegrees(direction);
		direction = Vector2::Degrees(Vector2::Reflect(a, Vector2::Up()));
		position = point;
	}
	if (position.x >= fieldSize.x) {
		Vector2 point = Vector2(fieldSize.x, position.y);
		Vector2 a = Vector2::FromDegrees(direction);
		direction = Vector2::Degrees(Vector2::Reflect(a, Vector2::Right()));
		position = point;
	}
	if (position.y >= fieldSize.y) {
		Vector2 point = Vector2(position.x, fieldSize.y);
		Vector2 a = Vector2::FromDegrees(direction);
		direction = Vector2::Degrees(Vector2::Reflect(a, Vector2::Down()));
		position = point;
	}
}
