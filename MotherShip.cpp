#include "MotherShip.h"
#include "Player.h"
#include "RNG.hpp"
#include "SceneManager.hpp"

// BabyShip functions
void BabyShip::Awake()
{
	score = 10;
	if (!texture.loadFromFile("assets/sprites/babyship.png")) {
		std::cerr << "Error loading baby ship texture\n";
		return;
	}
	sprite.setTexture(texture);
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);

	sf::FloatRect bounds = sprite.getGlobalBounds();
	Rect rect(Vector2(bounds.left, bounds.top), Vector2(bounds.left + bounds.width, bounds.top + bounds.height));
	hitbox = new Hitbox(position, { rect }, 0, 1);
}

void BabyShip::Start()
{
}

void BabyShip::Update()
{
	if (inactive) return;

	Vector2 targetPosition = position + Vector2::FromDegrees(angle);
	if (searching) targetPosition = Player::GetActivePlayer()->position;
	else if (returning) targetPosition = motherShip->position;

	// rotate towards the target position using rotationSpeed
	float targetAngle = Vector2::Degrees(Vector2::Direction(position, targetPosition));
	float angleDiff = targetAngle - angle;
	if (angleDiff > 180) angleDiff -= 360;
	if (angleDiff < -180) angleDiff += 360;
	if (std::abs(angleDiff) > rotationSpeed) {
		if (angleDiff > 0) angle += rotationSpeed;
		else angle -= rotationSpeed;
	}
	else angle = targetAngle;

	// Check if target is in front
	if (std::abs(angleDiff) < 120) speed = std::min(maxSpeed, speed + acceleration);
	else speed = std::max(1.0f, speed - acceleration);

	position = position + Vector2::FromDegrees(angle) * speed;

	if (returning) {
		if (Vector2::Distance(position, motherShip->position) < 10) {
			inactive = true;
			clipped = true;
			tag = 0;
		}
	}

	hitbox->SetCenter(position);
	hitbox->CalculateHitbox();
}

void BabyShip::Draw(sf::RenderTarget& target)
{
	if (clipped) return;

	sprite.setRotation(angle);
	sprite.setPosition(position.x, position.y);
	target.draw(sprite);
}

void BabyShip::OnDestroy()
{
	if (motherShip)
		motherShip->RemoveBaby(this);
}

// MotherShip functions
void MotherShip::Awake()
{
	score = 100;
	if (!texture.loadFromFile("assets/sprites/mothership.png")) {
		std::cerr << "Error loading mother ship texture\n";
		return;
	}
	sprite.setTexture(texture);
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);

	sf::FloatRect bounds = sprite.getGlobalBounds();
	Rect rect(Vector2(bounds.left, bounds.top), Vector2(bounds.left + bounds.width, bounds.top + bounds.height));
	hitbox = new Hitbox(position, { rect }, 0, 1);
}

void MotherShip::Start()
{
	CalculateTargetPosition();
	timer = 5;

	for (int i = 0; i < babies.size(); i++) {
		BabyShip* baby = new BabyShip();
		baby->motherShip = this;
		baby->inactive = true;
		baby->clipped = true;
		babies[i] = baby;
		SceneManager::GetInstance()->GetActiveScene()->AddObject(baby);
	}
}

void MotherShip::Update()
{
	if (inactive) return;

	timer -= Time::GetInstance()->GetDeltaTime();
	if (timer <= 0) {
		switch (mode) {
		// Shoot out babies 
		case 0: {
			int startAngle = -45;
			for (auto& baby : babies) {
				startAngle += 45;
				if (baby == nullptr) continue;
				baby->inactive = false;
				baby->clipped = false;
				baby->position = position;
				baby->angle = startAngle;
				baby->speed = 0;
				baby->searching = false;
				baby->returning = false;
				baby->tag = 1;
			}
			CalculateTargetPosition();
			timer = 1.0f;
			mode++;
			break;
		}
		// Make babies search for the player
		case 1: {
			for (auto& baby : babies) {
				if (baby == nullptr) continue;
				baby->searching = true;
				baby->returning = false;
			}
			timer = 5.0f;
			mode++;
			break;
		}
		// Make babies return to the mother ship
		case 2: {
			for (auto& baby : babies) {
				if (baby == nullptr) continue;
				baby->searching = false;
				baby->returning = true;
			}
			mode++;
			timer = 1.0f;
			break;
		}
		// Wait for babies to return
		case 3: {
			int returnedBabies = 0;
			int totalBabies = 0;
			for (auto& baby : babies) {
				if (baby == nullptr) continue;
				totalBabies++;
				if (baby->inactive) returnedBabies++;
			}
			if (returnedBabies == totalBabies) mode++;
			break;
		}
		// Reset mode
		case 4: {
			mode = 0;
			timer = 5.0f;
			break;
		}

		default:
			mode = 2;
			break;
		}
	}

	// Move over the curve from position to targetPosition past curvePosition
	t += Time::GetInstance()->GetDeltaTime() * std::min(0.2f + std::max(t, 0.0f), 0.6f + (t >= 0.7f ? 0.6f - t : 0));
	if (t > 1.0f)
		CalculateTargetPosition();

	// Quadratic Bezier curve formula: B(t) = (1-t)²P₀ + 2(1-t)tP₁ + t²P₂
	if (t >= 0)
		position = startPosition * (1 - t) * (1 - t) + curvePosition * (2 * (1 - t) * t) + targetPosition * (t * t);

	if (!Playfield::GetInstance()->IsInBounds(position, true))
		CalculateTargetPosition();

	hitbox->SetCenter(position);
	hitbox->CalculateHitbox();
}

void MotherShip::OnDestroy()
{
	for (auto& baby : babies) {
		if (baby != nullptr)
			Destroy(baby);
	}
	babies.fill(nullptr);
}

void MotherShip::CalculateTargetPosition()
{
	t = -0.2f;
	startPosition = position;

	float distance = RNG::GetRange(70, 120);
	std::unordered_map<Vector2, Vector2> points;
	std::vector<Vector2> dirs = {
		Vector2(1, 2), Vector2(2, 1), Vector2(1, -2), Vector2(-2, -1),
		Vector2(-1, 2),	Vector2(-2, 1),	Vector2(-1, -2), Vector2(2, -1)
	};

	// Calculate all possible target positions from dir
	for (auto dir : dirs) {
		Vector2 target = startPosition + dir * distance;
		points[dir] = target;
	}

	// Remove points that are out of bounds
	for (auto it = points.begin(); it != points.end();) {
		if (!Playfield::GetInstance()->IsInBounds(it->second, true))
			it = points.erase(it);
		else it++;
	}

	// If no valid points
	if (points.empty()) {
		if (Vector2::Distance(startPosition, Playfield::GetInstance()->GetClosestEdgePoint(startPosition)) < Vector2::Distance(startPosition, Playfield::GetInstance()->GetClosestWallPoint(startPosition)))
			targetPosition = Playfield::GetInstance()->GetClosestEdgePoint(startPosition);
		else
			targetPosition = Playfield::GetInstance()->GetClosestWallPoint(startPosition);

		curvePosition = startPosition;
		return;
	}

	int index = RNG::GetRange(0, points.size() - 1);
	auto it = points.begin();
	std::advance(it, index);
	Vector2 direction = it->first;
	Vector2 point = it->second;
	targetPosition = point;
	if (std::abs(direction.x) == 1) curvePosition = Vector2(startPosition.x + direction.x * distance, startPosition.y);
	else curvePosition = Vector2(startPosition.x, startPosition.y + direction.y * distance);
}
