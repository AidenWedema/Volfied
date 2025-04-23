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
}

void Snake::Update()
{
	if (inactive) return;

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