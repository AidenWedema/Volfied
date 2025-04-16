#include "Ship.h"
#include "Debug.hpp"

void Ship::Awake()
{
	score = 25;
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
	std::vector<Vector2>* p = Playfield::GetInstance()->GetWall();
	Vector2 nextPoint = position + Vector2::FromDegrees(direction) * speed;
	Vector2 point = Vector2();
	int index;
	if (Line::Intersects(Line(nextPoint, position), Line::CreateLineList(*p), point, index, true)) {
		Vector2 a = Vector2::FromDegrees(direction);
		Vector2 normal = Vector2::Normalize(p->at(index + 1) - p->at(index));
		direction = Vector2::Degrees(Vector2::Reflect(a, normal)) + 180;
		position = point;
	}
	Vector2 pos = position;
	if (!Playfield::GetInstance()->IsInBounds(position, true)) {
		Vector2 a = Vector2::FromDegrees(direction);
		Vector2 normal = Vector2::Normalize(pos - position);
		direction = Vector2::Degrees(Vector2::Reflect(a, normal));
	}
}
