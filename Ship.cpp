#include "Ship.h"
#include "Debug.hpp"

void Ship::Awake()
{
	score = 25;/*
	if (!texture.loadFromFile("assets/sprites/ship.png")) {
		std::cerr << "Error loading ship texture\n";
		return;
	}
	sprite.setTexture(texture);
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);*/
	animator.AddAnimation("Fly", "assets/sprites/ship.png");
	animator.SetAnimation(0);
}

void Ship::Start()
{
	direction = RNG::GetRange(0.0f, 22.5f) * 16 + 11.25f;
}

void Ship::Update()
{
	if (inactive) return;

	bounceTimer -= Time::GetInstance()->GetDeltaTime();
	if (bounceTimer <= 0) {
		bounceTimer = RNG::GetRange(30.0f, 60.0f);
		Vector2 a = Vector2::FromDegrees(direction);
		if (RNG::GetRand() < 4096)
			direction = Vector2::Degrees(Vector2::Reflect(a, Vector2::Up()));
		else
			direction = Vector2::Degrees(Vector2::Reflect(a, Vector2::Right()));
	}

	position = position + Vector2::FromDegrees(direction) * speed;

	// Keep the enemy in bounds
	Vector2 pos = position;
	if (!Playfield::GetInstance()->IsInBounds(position, true)) {
		Vector2 a = Vector2::FromDegrees(direction);
		Vector2 normal = Vector2::Normalize(pos - position);
		direction = Vector2::Degrees(Vector2::Reflect(a, normal));
	}
}

void Ship::Draw(sf::RenderTarget& target)
{
	if (clipped) return;

	animator.position = position;
	animator.Draw(target);
}

