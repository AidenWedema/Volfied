#include "Enemy.h"
#include "Game.h"

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
	position = position + Vector2::Rotate(Vector2::Up() * speed, angle);

	// Keep the player in bounds
	sf::Vector2u windowSize = Game::GetInstance()->GetWindow()->getSize();
	sf::Vector2f size = sprite.getLocalBounds().getSize();
	if (position.x <= 0) {
		position.x = 0;
		angle += 180;
	}
	if (position.y <= 0) {
		position.y = 0;
		angle += 180;
	}
	if (position.x >= windowSize.x - size.x) {
		position.x = windowSize.x - size.x;
		angle += 180;
	}
	if (position.y >= windowSize.y - size.y) {
		position.y = windowSize.y - size.y;
		angle += 180;
	}
	if (IsTouchingPath()) {
		angle += 180;
	}
}

void Enemy::Draw(sf::RenderTarget& target)
{
	sprite.setPosition(position.x, position.y);
	sprite.setRotation(angle);
	target.draw(sprite);
}

bool Enemy::IsTouchingPath()
{
	Player* player = Player::GetActivePlayer();
	std::vector<Vector2> path = *player->GetPath();
	path.push_back(player->position);

	for (auto& point : Playfield::GetInstance()->GetWall())
	{
		path.push_back(point);
	}

	if (path.size() < 2) return false;

	Vector2 lastPoint = position + Vector2::Rotate(Vector2::Up() * speed * 5, angle);
	Vector2 currentPoint = position;

	// Check against all previous segments (excluding the last one)
	for (size_t i = 0; i < path.size() - 1; i++)
	{
		Vector2 a = path[i];
		Vector2 b = path[i + 1];

		float minX = std::min(a.x, b.x);
		float minY = std::min(a.y, b.y);
		float maxX = std::max(a.x, b.x);
		float maxY = std::max(a.y, b.y);
		Vector2 minPoint = Vector2(std::min(lastPoint.x, currentPoint.x), std::min(lastPoint.y, currentPoint.y));
		Vector2 maxPoint = Vector2(std::max(lastPoint.x, currentPoint.x), std::max(lastPoint.y, currentPoint.y));
		if ((minY > currentPoint.y || currentPoint.y > maxY || minY > lastPoint.y || lastPoint.y > maxY) && a.x == b.x) continue;
		if ((minX > currentPoint.x || currentPoint.x > maxX || minX > lastPoint.x || lastPoint.x > maxX) && a.y == b.y) continue;
		if ((minPoint.x <= a.x && maxPoint.x >= a.x) || (minPoint.y <= a.y && maxPoint.y >= a.y))
		{
			return true;
		}
	}
	return false;
}