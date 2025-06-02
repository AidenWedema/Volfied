#include "LineFollower.h"
#include "Debug.hpp"

void LineFollower::Awake()
{
	if (!texture.loadFromFile("assets/sprites/line_follower.png")) {
		std::cerr << "Error loading line follower texture\n";
		return;
	}
	sprite.setTexture(texture);
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);

	sf::FloatRect bounds = sprite.getGlobalBounds();
	Rect rect(Vector2(bounds.left, bounds.top), Vector2(bounds.left + bounds.width, bounds.top + bounds.height));
	hitbox = new Hitbox(position, { rect }, 0, 1);
}

void LineFollower::Start()
{
	if (path == nullptr)
		Destroy(this);
}

void LineFollower::Update()
{
	if (inactive) return;

	if (index >= path->size()) {
		Destroy(this);
		return;
	}

	Vector2 target = path->at(index);
	position = Vector2::MoveTowards(position, target, speed);
	if (position == target){
		index++;
	}

	hitbox->SetCenter(position);
	hitbox->CalculateHitbox();
}