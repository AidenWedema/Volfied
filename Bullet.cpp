#include "Bullet.h"
#include "Player.h"

void Bullet::Awake()
{
	if (!texture.loadFromFile("assets/sprites/bullet.png")) {
		std::cerr << "Error loading bullet texture\n";
		return;
	}
	sprite.setTexture(texture);
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);

	sf::FloatRect bounds = sprite.getGlobalBounds();
	Rect rect(Vector2(bounds.left, bounds.top), Vector2(bounds.left + bounds.width, bounds.top + bounds.height));
	hitbox = new Hitbox(position, { rect }, 0, 1);
}

void Bullet::Start()
{
	sprite.setRotation(Vector2::Degrees(direction));
}

void Bullet::Update()
{
	if (inactive) return;

	position = position + direction * speed;

	Vector2 pos = position;
	if (!Playfield::GetInstance()->IsInBounds(position, true)) {
		if (bounceCount > 0) {
			bounceCount--;
			Vector2 a = direction;
			Vector2 normal = Vector2::Normalize(pos - position);
			direction = Vector2::Reflect(a, normal);
		}
		else Destroy(this);
	}

	hitbox->SetCenter(position);
	hitbox->CalculateHitbox();
}