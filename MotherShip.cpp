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
	angle = RNG::GetRange(0.0f, 11.25f) * 31;
	timer = 10;

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
			angle = RNG::GetRange(0.0f, 11.25f) * 31;
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

	position = position + Vector2::FromDegrees(angle) * speed;

	Vector2 pos = position;
	if (!Playfield::GetInstance()->IsInBounds(position, true)) {
		Vector2 a = Vector2::FromDegrees(angle);
		Vector2 normal = Vector2::Normalize(pos - position);
		angle = Vector2::Degrees(Vector2::Reflect(a, normal));
	}

	hitbox->SetCenter(position);
	hitbox->CalculateHitbox();
}
