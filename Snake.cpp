#include "Snake.h"
#include "Game.h"
#include "Debug.hpp"

void Snake::Awake()
{
	score = 50;
	if (!texture.loadFromFile("assets/sprites/snake.png")) {
		std::cerr << "Error loading snake texture\n";
		return;
	}
	sprite.setTexture(texture);
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
	if (!segmentTexture.loadFromFile("assets/sprites/snake_segment.png")) {
		std::cerr << "Error loading snake segment texture\n";
		return;
	}
	segmentSprite.setTexture(segmentTexture);
	segmentSprite.setOrigin(segmentSprite.getLocalBounds().width / 2, segmentSprite.getLocalBounds().height / 2);
}

void Snake::Start()
{
	for (int i = 0; i < length; i++)
	{
		SnakeSegment segment(this, segmentOffset * i, position, &path, &segmentSprite);
		segments.push_back(segment);
	}
	for (int i = 0; i < length * segmentOffset; i++)
		path.push_back(position);

	counterClockwise = RNG::GetRange(0, 1) == 0;
	coilCooldown = RNG::GetRange(coilCooldownRange.x, coilCooldownRange.y);
}

void Snake::Update()
{
	if (inactive) return;

	coilCooldown -= Time::GetInstance()->GetDeltaTime();
	if (coilCooldown <= 0 && coilTimer <= 0) {
		coilTimer = RNG::GetRange(coilTimerRange.x, coilTimerRange.y);
	}
	else if (coilTimer > 1) {
		coilTimer -= Time::GetInstance()->GetDeltaTime();
		if (coilTimer <= 1)
			coilTimer = 1;
		// Rotate towards the player
		Player* player = Player::GetActivePlayer();
		Vector2 playerPos = player->position;
		Vector2 dir = Vector2::Normalize(playerPos - position);
		float angleToPlayer = Vector2::Degrees(Vector2::Direction(position, playerPos));
		float angleDiff = angleToPlayer - angle;
		if (angleDiff < -180) angleDiff += 360;
		if (angleDiff > 180) angleDiff -= 360;
		if (angleDiff > 0) angle += rotationSpeed;
		else angle -= rotationSpeed;

		path.push_back(position);
		if (path.size() > length * segmentOffset + 1) {
			path.erase(path.begin());
		}
		Vector2 pos = position - Vector2::FromDegrees(angle);
		for (auto& segment : segments) {
			if (Vector2::Distance(path[segment.index], position) > 1)
				segment.Update();
			else {
				segment.position = pos;
				pos = pos - Vector2::FromDegrees(angle);
			}
		}
		return;
	}
	else if (coilTimer > 0) {
		coilTimer -= Time::GetInstance()->GetDeltaTime();

		position = position + Vector2::Rotate(Vector2::Up() * speed, angle);
		path.push_back(position);
		position = position + Vector2::Rotate(Vector2::Up() * speed, angle);
		Vector2 pos = position;
		if (!Playfield::GetInstance()->IsInBounds(position, true)) {
			Vector2 a = Vector2::FromDegrees(angle);
			Vector2 normal = Vector2::Normalize(pos - position);
			angle = Vector2::Degrees(Vector2::Reflect(a, normal));
			coilTimer = 0;
		}
		path.push_back(position);
		while (path.size() > length * segmentOffset + 1) {
			path.erase(path.begin());
		}
		for (auto& segment : segments) {
			segment.Update();
		}

		if (coilTimer <= 0)
			coilCooldown = RNG::GetRange(coilCooldownRange.x, coilCooldownRange.y);
		return;
	}

	turnAroundTimer -= Time::GetInstance()->GetDeltaTime();
	if (turnAroundTimer <= 0) {
		turnAroundTimer = RNG::GetRange(0.5f, 3.0f);
		counterClockwise = !counterClockwise;
	}

	rotationSpeedTimer -= Time::GetInstance()->GetDeltaTime();
	if (rotationSpeedTimer <= 0) {
		rotationSpeedTimer = RNG::GetRange(2.0f, 5.0f);
		rotationSpeedChange *= -1;
	}

	rotationSpeed += rotationSpeedChange;
	rotationSpeed = std::clamp(rotationSpeed, rotationSpeedRange.x, rotationSpeedRange.y);

	angle += counterClockwise ? -rotationSpeed : rotationSpeed;
	angle = angle % 360;
	if (angle < 0) angle += 360;

	Vector2 lastPos = position;
	position = position + Vector2::Rotate(Vector2::Up() * speed, angle);

	// Keep the enemy in bounds
	Vector2 point = Vector2();
	int index;
	Vector2 pos = position;
	if (!Playfield::GetInstance()->IsInBounds(position, true)) {
		Vector2 a = Vector2::FromDegrees(angle);
		Vector2 normal = Vector2::Normalize(pos - position);
		angle = Vector2::Degrees(Vector2::Reflect(a, normal));
	}
	path.push_back(position);
	if (path.size() > length * segmentOffset + 1) {
		path.erase(path.begin());
	}
	for (auto& segment : segments) {
		segment.Update();
	}
}

void Snake::Draw(sf::RenderTarget& target)
{
	if (clipped) return;

	for (auto& segment : segments) {
		segment.Draw(target);
	}
	sprite.setPosition(position.x, position.y);
	sprite.setRotation(angle);
	target.draw(sprite);
}

bool Snake::IsTouching(const Object& other) const {
	if (Object::IsTouching(other))
		return true;
	for (auto& segment : segments) {
		segment.sprite->setPosition(segment.position.x, segment.position.y);
		if (segment.sprite->getGlobalBounds().intersects(other.sprite.getGlobalBounds()) || segment.sprite->getGlobalBounds().contains(other.position.x, other.position.y))
			return true;
	}
	return false;
}