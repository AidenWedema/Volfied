#include "Canon.h"
#include "SceneManager.hpp"

void Canon::Awake()
{
	score = 40;
	if (!texture.loadFromFile("assets/sprites/canon.png")) {
		std::cerr << "Error loading canon texture\n";
		return;
	}
	sprite.setTexture(texture);
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 4 * 3);
	
	sf::FloatRect bounds = sprite.getGlobalBounds();
	Rect rect1(Vector2(-10, -32), Vector2(10, 0));
	Rect rect2(Vector2(-15, -15), Vector2(15, 15));
	hitbox = new Hitbox(position, { rect1, rect2 }, 0, 10);
}

void Canon::Start()
{
	direction = Vector2(RNG::GetRange(-1, 1), RNG::GetRange(-1, 1));
	shootTimer = RNG::GetRange(shootTime.x, shootTime.y);
	turnTimer = RNG::GetRange(turnTime.x, turnTime.y);
}

void Canon::Update()
{
	if (inactive) return;

	// Shoot
	turnTimer -= Time::GetInstance()->GetDeltaTime();
	shootTimer -= Time::GetInstance()->GetDeltaTime();
	if (shootTimer <= 0) {
		int angle = Vector2::Degrees(direction);
		for (int i = 0; i < 3; ++i) {
			Bullet* bullet = new Bullet();
			bullet->tag = tag;
			bullet->position = (position - Vector2(0, 32)).RotateAround(position, angle);
			bullet->direction = Vector2::FromDegrees(angle - 45 + (45 * i));
			bullet->bounceCount = 0;
			SceneManager::GetInstance()->GetActiveScene()->AddObject(bullet);
		}
		shootTimer = RNG::GetRange(shootTime.x, shootTime.y);
	}

	// Get closest wall and edge points
	Vector2 closestWallPoint = Playfield::GetInstance()->GetClosestWallPoint(position);
	Vector2 closestEdgePoint = Playfield::GetInstance()->GetClosestEdgePoint(position);

	// Calculate distances
	float wallDistance = Vector2::Distance(position, closestWallPoint);
	float edgeDistance = Vector2::Distance(position, closestEdgePoint);
	float currentAngle = Vector2::Degrees(direction);

	// If we're right on a wall or edgeinstantly turn 45 degrees right
	if (wallDistance < 0.1f || edgeDistance < 0.1f) {
		direction = Vector2::FromDegrees(currentAngle + 45);
		turnTimer = RNG::GetRange(turnTime.x, turnTime.y) * 0.5f;
	}
	if (turnTimer <= 0) {
		// Direction vectors away from obstacles
		Vector2 directionFromWall = Vector2::Direction(closestWallPoint, position);
		Vector2 directionFromEdge = Vector2::Direction(closestEdgePoint, position);

		const float avoidanceDistance = 100.0f;
		bool avoidingObstacle = false;

		if (wallDistance < avoidanceDistance || edgeDistance < avoidanceDistance) {
			// Determine which obstacle is closer
			Vector2 avoidDirection;

			if (wallDistance < edgeDistance)
				avoidDirection = directionFromWall;
			else
				avoidDirection = directionFromEdge;

			float targetAngle = Vector2::Degrees(avoidDirection);
			float angleDiff = targetAngle - currentAngle;
			if (angleDiff > 180) angleDiff -= 360;
			if (angleDiff < -180) angleDiff += 360;

			// Turn in 45-degree increments toward the avoidance direction
			if (angleDiff > 0)
				direction = Vector2::FromDegrees(currentAngle + 45);
			else
				direction = Vector2::FromDegrees(currentAngle - 45);
			avoidingObstacle = true;
		}

		// If not avoiding obstacles, use original wandering behavior
		if (!avoidingObstacle) {
			if (RNG::GetRange(0, 1) == 0)
				direction = Vector2::FromDegrees(currentAngle + 45);
			else
				direction = Vector2::FromDegrees(currentAngle - 45);
		}

		turnTimer = RNG::GetRange(turnTime.x, turnTime.y);
	}

	position = position + direction * speed;
	Playfield::GetInstance()->IsInBounds(position, true);

	hitbox->SetCenter(position);
	hitbox->CalculateHitbox(Vector2::Degrees(direction));
}


void Canon::Draw(sf::RenderTarget& target)
{
	if (clipped) return;

	sprite.setPosition(position.x, position.y);
	sprite.setRotation(Vector2::Degrees(direction));
	target.draw(sprite);
}
