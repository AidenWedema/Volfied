#include "Laser.h"
#include "SceneManager.hpp"
#include "Playfield.h"
#include "Score.hpp"

void Laser::Awake()
{
	if (!texture.loadFromFile("assets/sprites/laser.png")) {
		std::cerr << "Error loading laser texture\n";
		return;
	}
	sprite.setTexture(texture);
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
}

void Laser::Start()
{
	sprite.setRotation(Vector2::Degrees(direction));
}

void Laser::Update()
{
	if (inactive) return;

	position = position + direction * speed;

	for (auto& obj : SceneManager::GetInstance()->GetActiveScene()->GetObjectsWithTag(1)) {
		if (IsTouching(*obj)) {
			for (auto& subtag : obj->subTags) {
				if (subtag == 2) return;
			}
			Score::Add(obj->score);
			Destroy(obj);
			Destroy(this);
			break;
		}
	}

	if (!Playfield::GetInstance()->IsInBounds(position, false)) {
		Destroy(this);
	}
}

void Laser::Draw(sf::RenderTarget& target)
{
	if (clipped) return;
	sprite.setPosition(position.x, position.y);
	target.draw(sprite);
}