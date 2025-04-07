#include "Snake.h"
#include "Game.h"
#include "Debug.hpp"

void Snake::Awake()
{
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
		SnakeSegment segment(this, segmentOffset * (i + 1), position, &path, &segmentSprite);
		segments.push_back(segment);
	}
	for (int i = 0; i < length * segmentOffset; i++)
		path.push_back(position);
}

void Snake::Update()
{
	turnAroundTimer -= Time::GetInstance()->GetDeltaTime();
	if (turnAroundTimer <= 0) {
		turnAroundTimer = RNG::GetRange(0.5f, 3.0f);
		rotationSpeed *= -1;
	}

	angle += rotationSpeed;
	angle = angle % 360;
	if (angle < 0) angle += 360;
	position = position + Vector2::Rotate(Vector2::Up() * speed, angle);

	// Keep the enemy in bounds
	Vector2 fieldSize = Playfield::GetInstance()->GetSize();
	if (position.x <= 0) {
		Vector2 point = Vector2(0, position.y);
		Vector2 a = Vector2::FromDegrees(angle);
		angle = Vector2::Degrees(Vector2::Reflect(a, Vector2::Left()));
		position = point;
	}
	if (position.y <= 0) {
		Vector2 point = Vector2(position.x, 0);
		Vector2 a = Vector2::FromDegrees(angle);
		angle = Vector2::Degrees(Vector2::Reflect(a, Vector2::Up()));
		position = point;
	}
	if (position.x >= fieldSize.x) {
		Vector2 point = Vector2(fieldSize.x, position.y);
		Vector2 a = Vector2::FromDegrees(angle);
		angle = Vector2::Degrees(Vector2::Reflect(a, Vector2::Right()));
		position = point;
	}
	if (position.y >= fieldSize.y) {
		Vector2 point = Vector2(position.x, fieldSize.y);
		Vector2 a = Vector2::FromDegrees(angle);
		angle = Vector2::Degrees(Vector2::Reflect(a, Vector2::Down()));
		position = point;
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
	sprite.setPosition(position.x, position.y);
	sprite.setRotation(angle);
	target.draw(sprite);
	for (auto& segment : segments) {
		segment.Draw(target);
	}
}