#include "Stinger.h"
#include "Game.h"

void Stinger::Awake()
{
	if (!texture.loadFromFile("assets/sprites/stinger.png")) {
		std::cerr << "Error loading stinger texture\n";
		return;
	}
	sprite.setTexture(texture);
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
	sprite.setScale(0.5f, 0.5f);
}

void Stinger::Start()
{
}

void Stinger::Update()
{
	if (inactive) return;

	int newAngle = Vector2::Degrees(Vector2::Direction(position, Player::GetActivePlayer()->position));
	if (newAngle < 0) newAngle += 360;
	int diff = newAngle - angle;

	if (std::abs(diff) > 45) speed = std::max(0.5f, speed - 0.1f);
	else speed = std::min(maxSpeed, speed + acceleration);

	if (diff > 180) diff -= 360;
	else if (diff < -180) diff += 360;
	if (diff > maxRotation) diff = maxRotation;
	else if (diff < -maxRotation) diff = -maxRotation;
	angle += diff;

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
	}
	Vector2 pos = position;
	if (!Playfield::GetInstance()->IsInBounds(position, true)) {
		Vector2 a = Vector2::FromDegrees(angle);
		Vector2 normal = Vector2::Normalize(pos - position);
		angle = Vector2::Degrees(Vector2::Reflect(a, normal));
	}
}

void Stinger::Draw(sf::RenderTarget& target)
{
	if (clipped) return;
	sprite.setPosition(position.x, position.y);
	sprite.setRotation(angle);
	target.draw(sprite);
}
