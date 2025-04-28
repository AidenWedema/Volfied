#include "Enemy.h"
#include "Game.h"
#include "Debug.hpp"

void Enemy::Awake()
{
	score = 20;
	if (!texture.loadFromFile("assets/sprites/enemy.png")) {
		std::cerr << "Error loading enemy texture\n";
		return;
	}
	sprite.setTexture(texture);
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
}

void Enemy::Start()
{
}

void Enemy::Update()
{
	if (inactive) return;

	turnAroundTimer -= Time::GetInstance()->GetDeltaTime();
	if (turnAroundTimer <= 0) {
		turnAroundTimer = RNG::GetRange(1.0f, 4.0f);
		counterClockwise = !counterClockwise;
	}

	rotationSpeedTimer -= Time::GetInstance()->GetDeltaTime();
	if (rotationSpeedTimer <= 0) {
		rotationSpeedTimer = RNG::GetRange(0.5f, 3.0f);
		rotationSpeedChange *= -1;
	}

	rotationSpeed += rotationSpeedChange;
	rotationSpeed = std::clamp(rotationSpeed, rotationSpeedRange.x, rotationSpeedRange.y);

	angle += counterClockwise ? -rotationSpeed : rotationSpeed;
	angle = angle % 360;
	if (angle < 0) angle += 360;
	position = position + Vector2::Rotate(Vector2::Up() * speed, angle);

	// Keep the enemy in bounds
	Player* player = Player::GetActivePlayer();
	std::vector<Vector2>* p = player->GetPath();
	Vector2 nextPoint = position + Vector2::Rotate(Vector2::Up() * speed, angle);
	Vector2 point = Vector2();
	int index;
	if (Line::Intersects(Line(nextPoint, position), Line::CreateLineList(*p), point, index, true)) {
		Vector2 a = Vector2::FromDegrees(angle);
		Vector2 dir = Vector2::Normalize(p->at(index + 1) - p->at(index));
		angle = Vector2::Degrees(Vector2::Reflect(a, dir)) + 180;
		position = point;
		SpawnLineFollower(point, index, player->GetPath());
	}
	Vector2 pos = position;
	if (!Playfield::GetInstance()->IsInBounds(position, true)) {
		Vector2 a = Vector2::FromDegrees(angle);
		Vector2 normal = Vector2::Normalize(pos - position);
		angle = Vector2::Degrees(Vector2::Reflect(a, normal));
	}
}

void Enemy::Draw(sf::RenderTarget& target)
{
	if (clipped) return;

	sprite.setPosition(position.x, position.y);
	sprite.setRotation(angle);
	target.draw(sprite);
}

void Enemy::SpawnLineFollower(Vector2 start, int index, std::vector<Vector2>* path)
{
	LineFollower* lf = dynamic_cast<LineFollower*>(Object::Instantiate("prefabs/LineFollower"));
	if (lf == nullptr) {
		Debug::Log("Error instantiating LineFollower");
		return;
	}
	lf->position = start;
	lf->index = index;
	lf->path = path;
}
