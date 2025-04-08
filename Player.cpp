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
	path.push_back(position);
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
	if (lastDirection != direction && direction != Vector2()) {
		path.push_back(position);
	}

	position = position + direction * speed;
	path[path.size() - 1] = position;
	if (path.size() > 1)
	{
		Vector2 lastPoint = path[path.size() - 2];
		if (Vector2::LineIntersects(lastPoint, position, path))
		{
			position = path[0];
			direction = Vector2();
			path.clear();
			path.push_back(position);
			path.push_back(position);
		}
	}

	// Keep the player in bounds
    if (Playfield::GetInstance()->IsInBounds(position, sprite.getGlobalBounds(), true)) {
		lastDirection = Vector2();
		if (path.size() > 1) {
			path[path.size() - 1] = position;
			direction = Vector2();
			Playfield::GetInstance()->AddWall(path);
			path.clear();
			path.push_back(position);
			path.push_back(position);
		}
    }
	for (auto& enemy : SceneManager::GetInstance()->GetActiveScene()->GetObjectsWithTag(1)) {
		if (enemy->IsTouching(*this) && !path.empty() && Time::GetInstance()->GetFrameCount() > 1) {
			position = path[0];
			direction = Vector2();
			path.clear();
			path.push_back(position);
			path.push_back(position);
			Debug::Log("DED!");
			break;
		}
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
