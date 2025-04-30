#include "Turret.h"
#include "SceneManager.hpp"

void Turret::Awake()
{
	score = 10;
	if (!texture.loadFromFile("assets/sprites/turret.png")) {
		std::cerr << "Error loading turret texture\n";
		return;
	}
	sprite.setTexture(texture);
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
}

void Turret::Start()
{
	shootTimer = shootTime;
}

void Turret::Update()
{
	if (inactive) return;

	shootTimer -= Time::GetInstance()->GetDeltaTime();

	float t = shootTimer / shootTime;
	float angle = 360 * (1 - t) + directionChange;
	sprite.setRotation(angle);

	if (shootTimer <= 0) {
		shootTimer = shootTime;
		direction += directionChange;
		for (int i = 0; i < 4; i++) {
			Bullet* bullet = new Bullet();
			bullet->tag = tag;
			bullet->position = position;
			bullet->direction = Vector2::FromDegrees(direction);
			bullet->bounceCount = 2;
			SceneManager::GetInstance()->GetActiveScene()->AddObject(bullet);
			direction += 90;
		}
	}
}
