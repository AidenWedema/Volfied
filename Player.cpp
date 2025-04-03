#include "Player.h"
#include "Game.h"
#include "Debug.hpp"

Player* Player::activePlayer = nullptr;

void Player::Awake()
{
	if (!texture.loadFromFile("assets/sprites/player.png")) {
		std::cerr << "Error loading player texture\n";
		return;
	}
	sprite.setTexture(texture);
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
	activePlayer = this;
}

void Player::Start()
{
}

void Player::Update()
{
	if (direction != Vector2()) lastDirection = direction;

	Vector2 oppositeDirection = lastDirection * -1;
	if (Input::GetInstance()->GetKey("Left") && Vector2::Left() != oppositeDirection) {
		direction = Vector2::Left();
	} else if (Input::GetInstance()->GetKey("Right") && Vector2::Right() != oppositeDirection) {
		direction = Vector2::Right();
	} else if (Input::GetInstance()->GetKey("Up") && Vector2::Up() != oppositeDirection) {
		direction = Vector2::Up();
	} else if (Input::GetInstance()->GetKey("Down") && Vector2::Down() != oppositeDirection) {
		direction = Vector2::Down();
	} else {
		direction = Vector2();
	}

	// Add the current position to the path if the direction has changed
	if (lastDirection != direction) {
		path.push_back(position);
	}

	position = position + direction * speed;

	if (DoesPathOverlap())
	{
		position = path[0];
		direction = Vector2();
		path.clear();
	}

	// Keep the player in bounds
	bool reachedEdge = false;
	Vector2 fieldSize = Playfield::GetInstance()->GetSize();
	sf::Vector2f s = sprite.getGlobalBounds().getSize();
	Vector2 size = Vector2(s.x * 0.5f, s.y * 0.5f);
	if (position.x <= size.x) {
		position.x = 0 + size.x;
		reachedEdge = true;
	}
	if (position.y <= size.y) {
		position.y = 0 + size.y;
		reachedEdge = true;
	}
	if (position.x >= fieldSize.x - size.x) {
		position.x = fieldSize.x - size.x;
		reachedEdge = true;
	}
	if (position.y >= fieldSize.y - size.y) {
		position.y = fieldSize.y - size.y;
		reachedEdge = true;
	}
    if (reachedEdge) {
		lastDirection = Vector2();
		path.push_back(position);
		if (!path.empty()) Playfield::GetInstance()->AddWall(path);
        path.clear();
    }
}

void Player::Draw(sf::RenderTarget& target)
{
	sprite.setPosition(position.x, position.y);
	target.draw(sprite);

	// Draw the path
	sf::VertexArray line(sf::LineStrip, path.size() + 1);
	for (int i = 0; i < path.size(); i++) {
		line[i] = sf::Vertex(sf::Vector2f(path[i].x, path[i].y), sf::Color::White);
	}
	line[path.size()] = sf::Vertex(sf::Vector2f(position.x, position.y), sf::Color::White);
	target.draw(line);
}

bool Player::DoesPathOverlap()
{
	if (path.size() < 2) return false;

	Vector2 lastPoint = path[path.size() - 1];
	Vector2 currentPoint = position;

	Debug::DrawLine(lastPoint, currentPoint, sf::Color::Green);

	// Determine if the new segment is horizontal or vertical
	bool isHorizontal = (lastPoint.y == currentPoint.y);
	bool isVertical = (lastPoint.x == currentPoint.x);

	// Check against all previous segments (excluding the last one)
	for (size_t i = 0; i < path.size() - 1; i++)
	{
		Vector2 a = path[i];
		Vector2 b = path[i + 1];

		if (a.y == b.y && isHorizontal) continue; // Ignore parallel vertical lines
		if (a.x == b.x && isVertical) continue;   // Ignore parallel horizontal lines

		if (isHorizontal)
		{
			int minY = std::min(a.y, b.y);
			int maxY = std::max(a.y, b.y);
			// (lastPoint, currentPoint) is horizontal so we check for a vertical segment
			if (minY > lastPoint.y || lastPoint.y > maxY) continue;
			if (std::min(lastPoint.x, currentPoint.x) < a.x && std::max(lastPoint.x, currentPoint.x) > a.x) {
				return true;
			}
		}
		else if (isVertical)
		{
			int minX = std::min(a.x, b.x);
			int maxX = std::max(a.x, b.x);
			// (a, b) is horizontal, (lastPoint, currentPoint) is vertical
			if (minX > currentPoint.x || currentPoint.x > maxX) continue;
			if (std::min(lastPoint.y, currentPoint.y) < a.y && std::max(lastPoint.y, currentPoint.y) > a.y) {
				return true;
			}
		}
	}
	return false;
}
