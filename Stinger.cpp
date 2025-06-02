#include "Stinger.h"
#include "Game.h"

void Stinger::Awake()
{
	score = 35;

	animator.AddAnimation("Fly", "assets/sprites/stinger.png");
	animator.SetAnimation("Fly");
	animator.autoUpdate = false;

	sf::FloatRect bounds = sprite.getGlobalBounds();
	Rect rect(Vector2(-16, -16), Vector2(16, 16));
	hitbox = new Hitbox(position, { rect }, 0, 1);
}

void Stinger::Start()
{
}

void Stinger::Update()
{
	if (inactive) return;

	animator.current->Update();

	// Every 5 seconds, snap out of cunfusion or have a 1 in 3 chance to be confused.
	confusionTimer -= Time::GetInstance()->GetDeltaTime();
	if (confusionTimer <= 0) {
		confusionTimer = confusionTime;
		if (confused) {
			confused = false;
		}
		else if (RNG::GetRange(0, 3) == 0) {
			confused = true;
		}
	}

	// Get target direction
	Vector2 targetPos = Player::GetActivePlayer()->position;
	if (confused)
		targetPos = targetPos.RotateAround(Playfield::GetInstance()->position, 180);

	Vector2 toTarget = Vector2::Direction(position, targetPos);
	float newAngle = Vector2::Degrees(toTarget);
	
	// Normalize angles between [0, 360]
	newAngle = fmod(newAngle + 360.0f, 360.0f);
	angle = fmod(angle + 360.0f, 360.0f);

	// Calculate the shortest difference between angles (-180 to 180)
	float diff = newAngle - angle;
	if (diff > 180) diff -= 360;
	if (diff < -180) diff += 360;

	// Check if target is in front
	if (std::abs(diff) < 90)
		speed = std::min(maxSpeed, speed + acceleration);
	else
		speed = std::max(decceleration, speed - decceleration);

	// Rotate towards target
	if (diff > maxRotation)
		angle += maxRotation;
	else if (diff < -maxRotation)
		angle -= maxRotation;
	else
		angle = newAngle;

	angle = fmod(angle + 360.0f, 360.0f);
	Vector2 lastPos = position;
	position = position + Vector2::Rotate(Vector2::Up() * speed, angle);

	// Keep the enemy in bounds
	Player* player = Player::GetActivePlayer();
	std::vector<Vector2>* p = player->GetPath();
	Vector2 point = Vector2();
	int index;
	if (Line::Intersects(Line(lastPos, position), Line::CreateLineList(*p), point, index, true)) {
		Vector2 a = Vector2::FromDegrees(angle);
		Vector2 dir = Vector2::Normalize(p->at(index + 1) - p->at(index));
		angle = Vector2::Degrees(Vector2::Reflect(a, dir)) + 180;
		position = point + Vector2::Rotate(Vector2::Up() * speed, angle);
	}
	Vector2 pos = position;
	if (!Playfield::GetInstance()->IsInBounds(position, true)) {
		Vector2 a = Vector2::FromDegrees(angle);
		Vector2 normal = Vector2::Normalize(pos - position);
		angle = Vector2::Degrees(Vector2::Reflect(a, normal));
	}

	hitbox->SetCenter(position);
	hitbox->CalculateHitbox();
}

void Stinger::Draw(sf::RenderTarget& target)
{
	if (clipped) return;

	sprite = animator.current->sprite;
	sprite.setPosition(position.x, position.y);
	sprite.setRotation(angle);
	target.draw(sprite);
}
