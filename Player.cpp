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
	if (isDigging) Dig();
	else Walk();

	for (auto& enemy : SceneManager::GetInstance()->GetActiveScene()->GetObjectsWithTag(1)) {
		if (enemy->IsTouching(*this) && !path.empty() && Time::GetInstance()->GetFrameCount() > 1) {
			position = path[0];
			direction = Vector2();
			lastDirection = direction;
			path.clear();
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

void Player::Walk()
{
	if (Input::GetInstance()->GetKey("Left")) direction = Vector2::Left();
	else if (Input::GetInstance()->GetKey("Right")) direction = Vector2::Right();
	else if (Input::GetInstance()->GetKey("Up")) direction = Vector2::Up();
	else if (Input::GetInstance()->GetKey("Down")) direction = Vector2::Down();
	else direction = Vector2();
	if (direction == Vector2()) return;

	Vector2 newPosition = position + direction * speed;
	Vector2 closestWallPoint = Playfield::GetInstance()->GetClosestWallPoint(newPosition);
	Vector2 closestEdgePoint = Playfield::GetInstance()->GetClosestEdgePoint(newPosition);
	bool isCurOnWall = false;
	bool isNewOnWall = false;
	bool isCurOnEdge = false;
	bool isNewOnEdge = false;
	for (auto& point : Playfield::GetInstance()->GetExtentPoints()) {
		if (position.x == point.x || position.y == point.y) {
			isCurOnEdge = true;
			break;
		}
	}
	for (auto& point : Playfield::GetInstance()->GetExtentPoints()) {
		if (newPosition.x == point.x || newPosition.y == point.y) {
			isNewOnEdge = true;
			break;
		}
	}
	for (auto& area : *Playfield::GetInstance()->GetWallArea()) {
		if (area.OnEdge(position)) {
			isCurOnWall = true;
			break;
		}
	}
	for (auto& area : *Playfield::GetInstance()->GetWallArea()) {
		if (area.OnEdge(newPosition)) {
			isNewOnWall = true;
			break;
		}
	}

	// Stay on the edge of the playfield
	if (isCurOnEdge && isNewOnEdge) {
		if (!Playfield::GetInstance()->IsInBounds(newPosition, false)) {
			// Is the player moveing out of the playfield or into a wall?
			if (isNewOnWall) {
				Vector2 closestPoint = Vector2 (-100000, -100000);
				for (auto& area : *Playfield::GetInstance()->GetWallArea()) {
					Vector2 point = area.ClosestCorner(position);
					if (Vector2::Distance(point, position) < Vector2::Distance(closestPoint, position))
						closestPoint = point;
				}
				position = closestPoint;
			}
			else position = closestEdgePoint;
		}
		else if (!isCurOnWall && !isNewOnWall) position = newPosition;
		else if (!isCurOnWall && isNewOnWall) position = newPosition;
		else if (isCurOnWall && !isNewOnWall) position = newPosition;
		else if (isCurOnWall && isNewOnWall) {
			Vector2 closestPoint = Vector2(-100000, -100000);
			for (auto& area : *Playfield::GetInstance()->GetWallArea()) {
				Vector2 point = area.ClosestCorner(position);
				if (Vector2::Distance(point, position) < Vector2::Distance(closestPoint, position))
					closestPoint = point;
			}
			position = closestPoint;
		}
	}
	// Move off the edge of the playfield
	else if (isCurOnEdge && !isNewOnEdge) {
 		if (!Playfield::GetInstance()->IsInBounds(newPosition, false)) position = closestEdgePoint;
		else if (isCurOnWall && isNewOnWall) position = newPosition;
	}
	// Move onto the edge of the playfield
	else if (!isCurOnEdge && isNewOnEdge) {
		if (isCurOnWall && !isNewOnWall) position = newPosition;
		else 
			position = closestEdgePoint; // Should be impossible to get here
	}
	// Stay on the wall
	else if (!isCurOnEdge && !isNewOnEdge) {
		if (!Playfield::GetInstance()->IsInBounds(newPosition, false)) position = position;
		else if (isCurOnWall && isNewOnWall) position = newPosition;
		else if (!isCurOnWall && isNewOnWall) position = closestEdgePoint;
		else if (isCurOnWall && !isNewOnWall) position = position;
	}
	if (!Input::GetInstance()->GetKey("A")) return;
	isDigging = true;
	path.clear();
	path.push_back(position);
	path.push_back(position);
	position = newPosition;
}

void Player::Dig()
{
	Vector2 oppositeDirection = lastDirection * -1;
	if (Input::GetInstance()->GetKey("Left") && Vector2::Left() != oppositeDirection)
		direction = Vector2::Left();
	else if (Input::GetInstance()->GetKey("Right") && Vector2::Right() != oppositeDirection)
		direction = Vector2::Right();
	else if (Input::GetInstance()->GetKey("Up") && Vector2::Up() != oppositeDirection)
		direction = Vector2::Up();
	else if (Input::GetInstance()->GetKey("Down") && Vector2::Down() != oppositeDirection)
		direction = Vector2::Down();
	else direction = Vector2();

	// Add the current position to the path if the direction has changed
	if (lastDirection != direction && direction != Vector2()) {
		path.push_back(position);
	}

	position = position + direction * speed;
	path[path.size() - 1] = position;
	if (path.size() > 1)
	{
		Vector2 lastPoint = path[path.size() - 2];
		if (Line::Intersects(Line(lastPoint, position), Line::CreateLineList(path)))
		{
			position = path[0];
			direction = Vector2();
			path.clear();
			path.push_back(position);
		}
	}

	// Keep the player in bounds
	if (!Playfield::GetInstance()->IsInBounds(position, true)) {
		lastDirection = Vector2();
		if (path.size() > 1) {
			path[path.size() - 1] = position;
			direction = Vector2();
			Playfield::GetInstance()->AreaFill(path);
			path.clear();
			isDigging = false;
		}
	}
}
