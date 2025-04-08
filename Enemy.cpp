#include "Enemy.h"
#include "Game.h"
#include "Debug.hpp"

void Enemy::Awake()
{
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
	turnAroundTimer -= Time::GetInstance()->GetDeltaTime();
	if (turnAroundTimer <= 0) {
		turnAroundTimer = RNG::GetRange(1.0f, 4.0f);
		rotationSpeed *= -1;
	}

	angle += rotationSpeed;
	angle = angle % 360;
	if (angle < 0) angle += 360;
	position = position + Vector2::Rotate(Vector2::Up() * speed, angle);

	// Keep the enemy in bounds
	Player* player = Player::GetActivePlayer();
	std::vector<Vector2>* p = player->GetPath();
	Vector2 nextPoint = position + Vector2::Rotate(Vector2::Up() * speed, angle);
	Vector2 point = Vector2();
	int index;
	if (Vector2::LineIntersects(nextPoint, position, *p, point, index, true)) {
		Vector2 a = Vector2::FromDegrees(angle);
		Vector2 dir = Vector2::Normalize(p->at(index + 1) - p->at(index));
		angle = Vector2::Degrees(Vector2::Reflect(a, dir)) + 180;
		position = point;
		SpawnLineFollower(point, index, player->GetPath());
	}
	p = Playfield::GetInstance()->GetWall();
	if (Vector2::LineIntersects(nextPoint, position, *p, point, index, true)) {
		Vector2 a = Vector2::FromDegrees(angle);
		Vector2 dir = Vector2::Normalize(p->at(index + 1) - p->at(index));
		angle = Vector2::Degrees(Vector2::Reflect(a, dir)) + 180;
		position = point;
	}
	Vector2 pos = position;
	if (Playfield::GetInstance()->IsInBounds(position, sprite.getGlobalBounds(), true)) {
		Vector2 a = Vector2::FromDegrees(angle);
		Vector2 normal = Vector2::Normalize(pos - position);
		angle = Vector2::Degrees(Vector2::Reflect(a, normal));
	}
}

void Enemy::Draw(sf::RenderTarget& target)
{
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
