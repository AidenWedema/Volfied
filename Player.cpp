#include "Player.h"
#include "Game.h"
#include "Debug.hpp"
#include "AudioManager.hpp"
#include "DoomRay.h"

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

	sf::FloatRect bounds = sprite.getGlobalBounds();
	Rect rect(Vector2(bounds.left, bounds.top), Vector2(bounds.left + bounds.width, bounds.top + bounds.height));
	hitbox = new Hitbox(position, { rect }, 0, 1);
}

void Player::Start()
{
}

void Player::Update()
{
	if (inactive) return;

	if (respawnTimer > 0) {
		respawnTimer -= Time::GetInstance()->GetDeltaTime();
		Game::GetInstance()->SetPaused(true);
		inactive = false;
		if (respawnTimer < 1) {
			if (lives <= 0) {
				SceneManager::GetInstance()->LoadScene("GameOver");
			}
			clipped = false;
		} 
		if (respawnTimer <= 0) Game::GetInstance()->SetPaused(false);
		return;
	}

	if (direction != Vector2()) lastDirection = direction;
	if (isDigging) Dig();
	else Walk();

	std::vector<Powerup*> oldPowers;
	for (auto& power : powers) {
		power->Use();
		if (power->timer <= 0) {
			oldPowers.push_back(power);
		}
	}

	for (auto& power : oldPowers) {
		auto it = std::remove(powers.begin(), powers.end(), power);
		powers.erase(it, powers.end());
		delete power;
	}

	for (auto& enemy : SceneManager::GetInstance()->GetActiveScene()->GetObjectsWithTag(1)) {
		if (enemy->IsTouching(*this) && !path.empty()) {
			Die();
			break;
		}
	}

	hitbox->SetCenter(position);
	hitbox->CalculateHitbox();
}

void Player::Draw(sf::RenderTarget& target)
{
	if (clipped) return;

	sprite.setRotation(Vector2::Degrees(lastDirection));
	sprite.setPosition(position.x, position.y);
	target.draw(sprite);

	// Draw the path
	sf::VertexArray line(sf::LineStrip, path.size() + 1);
	for (int i = 0; i < path.size(); i++) {
		line[i] = sf::Vertex(sf::Vector2f(path[i].x, path[i].y), sf::Color::White);
	}
	line[path.size()] = sf::Vertex(sf::Vector2f(position.x, position.y), sf::Color::White);
	target.draw(line);

	// Draw the powerups
	for (auto& power : powers) {
		power->Draw(target);
	}

	// Draw life counter
	int x = 750;
	sprite.setRotation(0);
	sprite.setScale(3, 3);
	for (int i = 0; i < lives; i++) {
		sprite.setPosition(x, 550);
		target.draw(sprite);
		x -= 25;
	}
	sprite.setScale(1, 1);
}

void Player::Die()
{
	lives--;
	clipped = true;
	CutsceneObject* explosion = dynamic_cast<CutsceneObject*>(Object::Instantiate("prefabs/CutsceneObjects/explosion"));
	explosion->position = position;
	explosion->animator.SetAnimation(0);
	Game::GetInstance()->SetPaused(true);
	inactive = false;
	respawnTimer = 3;
	position = path[0];
	direction = Vector2();
	lastDirection = direction;
	path.clear();
	isDigging = false;
}

void Player::Walk()
{
	if (Input::GetInstance()->GetKey("Left")) direction = Vector2::Left();
	else if (Input::GetInstance()->GetKey("Right")) direction = Vector2::Right();
	else if (Input::GetInstance()->GetKey("Up")) direction = Vector2::Up();
	else if (Input::GetInstance()->GetKey("Down")) direction = Vector2::Down();
	else return;

	Vector2 newPosition = position + direction * speed;
	Vector2 closestWallPoint = Playfield::GetInstance()->GetClosestWallPoint(newPosition);
	Vector2 closestEdgePoint = Playfield::GetInstance()->GetClosestEdgePoint(newPosition);
	Vector2 closestCorner = Vector2(-100000, -100000);
	bool isCurOnWall = false;
	bool isNewOnWall = false;
	bool isCurOnEdge = false;
	bool isNewOnEdge = false;
	bool isCurSmushed = false;
	bool isNewSmushed = false;
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
			if (!isCurOnWall) isCurOnWall = true;
			else if (isCurOnWall) {
				isCurSmushed = true;
				break;
			}
		} 
	}
	for (auto& area : *Playfield::GetInstance()->GetWallArea()) {
		if (area.OnEdge(newPosition)) {
			if (!isNewOnWall) isNewOnWall = true;
			else {
				isNewSmushed = true;
				break;
			}
		} 
	}
	for (auto& area : *Playfield::GetInstance()->GetWallArea()) {
		Vector2 point = area.ClosestCorner(position);
		if (Vector2::Distance(point, position) < Vector2::Distance(closestCorner, position))
			closestCorner = point;
	}

	// Stay on the edge of the playfield
	if (isCurOnEdge && isNewOnEdge) {
		if (!Playfield::GetInstance()->IsInBounds(newPosition, false)) {
			// Is the player moveing out of the playfield or into a wall?
			if (isNewOnWall) position = closestCorner;
			else position = closestEdgePoint;
		}
		else if (!isCurOnWall && !isNewOnWall) position = newPosition;
		else if (!isCurOnWall && isNewOnWall) position = newPosition;
		else if (isCurOnWall && !isNewOnWall) position = newPosition;
		else if (isCurOnWall && isNewOnWall) {
			if (lastDirection != direction) position = newPosition;
			else position = closestCorner;
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
		if (isCurOnWall && isNewOnWall) {
			if (isCurSmushed && isNewSmushed) position = closestCorner;
			else position = newPosition;
		}
		else if (isCurOnWall && !isNewOnWall) position = closestWallPoint;
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
	if (Input::GetInstance()->GetKey("Left"))
		direction = Vector2::Left();
	else if (Input::GetInstance()->GetKey("Right"))
		direction = Vector2::Right();
	else if (Input::GetInstance()->GetKey("Up"))
		direction = Vector2::Up();
	else if (Input::GetInstance()->GetKey("Down"))
		direction = Vector2::Down();
	else direction = Vector2();

	// Add the current position to the path if the direction has changed
	if (lastDirection != direction && direction != lastDirection * -1 && direction != Vector2()) {
		path.push_back(position);
	}

	Vector2 newPosition = position + direction * speed;
	bool isOOB = false;
	if (!Playfield::GetInstance()->IsInBounds(newPosition, true))
		isOOB = true;

	position = newPosition;
	path[path.size() - 1] = position;
	if (path.size() > 1)
	{
		Vector2 lastPoint = path[path.size() - 2];
		if (Line::Intersects(Line(lastPoint, position), Line::CreateLineList(path)))
			Die();
	}

	// Keep the player in bounds
	if (isOOB) {
		direction = Vector2();
		lastDirection = Vector2();
		isDigging = false;
		if (path.size() == 2 && path[0] == path[path.size() - 1]) {
			path.clear();
			return;
		}
		if (path.size() > 1) {
			path[path.size() - 1] = position;
			Playfield::GetInstance()->AreaFill(path);
			path.clear();
		}
	}
}
