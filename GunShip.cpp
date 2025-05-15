#include "GunShip.h"
#include "SceneManager.hpp"

void GunShip::Awake()
{
	score = 30;
	if (!texture.loadFromFile("assets/sprites/gunship.png")) {
		std::cerr << "Error loading gunship texture\n";
		return;
	}
	sprite.setTexture(texture);
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
}

void GunShip::Start()
{
	direction = RNG::GetRange(0.0f, 22.5f) * 16 + 11.25f;
	shootTimer = shootTime.y;
}

void GunShip::Update()
{
	if (inactive) return;

	shootTimer -= Time::GetInstance()->GetDeltaTime();
	if (shootTimer <= 0) {
		if (shooting) {
			if (timesToShoot > 0) {
				Bullet* bullet = new Bullet();
				bullet->tag = tag;
				bullet->position = position;
				bullet->direction = Vector2::Direction(position, Player::GetActivePlayer()->position);
				bullet->bounceCount = 0;
				SceneManager::GetInstance()->GetActiveScene()->AddObject(bullet);
				shootTimer = 0.05f;
				timesToShoot--;
				if (timesToShoot <= 0) {
					shooting = false;
					shootTimer = Playfield::GetInstance()->GetPercentCleared() < 0.5f ? shootTime.y : shootTime.x;
				}
			}
		}
		else {
			Vector2 a = Vector2::FromDegrees(direction);
			direction = RNG::GetRange(0.0f, 22.5f) * 16 + 11.25f;
			shootTimer = 0.3f;
			timesToShoot = 3;
			shooting = true;
		}
	}

	if (shooting) return;

	position = position + Vector2::FromDegrees(direction) * speed;

	// Keep the enemy in bounds
	Vector2 pos = position;
	if (!Playfield::GetInstance()->IsInBounds(position, true)) {
		Vector2 a = Vector2::FromDegrees(direction);
		Vector2 normal = Vector2::Normalize(pos - position);
		direction = Vector2::Degrees(Vector2::Reflect(a, normal));
	}
}
