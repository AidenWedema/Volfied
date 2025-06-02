#include "Ship.h"
#include "Debug.hpp"

void Ship::Awake()
{
	score = 25;

	animator.AddAnimation("Fly", "assets/sprites/ship.png");
	animator.SetAnimation(0);
	animator.autoUpdate = false;

	Rect rect(Vector2(-16, -16), Vector2(16, 16));
	hitbox = new Hitbox(position, { rect }, 0, 1);
}

void Ship::Start()
{
	direction = RNG::GetRange(0.0f, 22.5f) * 16 + 11.25f;
}

void Ship::Update()
{
	if (inactive) return;

	animator.current->Update();

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

	hitbox->SetCenter(position);
	hitbox->CalculateHitbox();
}

void Ship::Draw(sf::RenderTarget& target)
{
	if (clipped) return;

	sprite = animator.current->sprite;
	sprite.setPosition(position.x, position.y);
	target.draw(sprite);
}

