#pragma once
#include "Powerup.hpp"
#include "Player.h"
#include "Playfield.h"
#include "Game.h"
#include "Debug.hpp"

using shape::Line;
using shape::Rect;

class DoomRay : public Powerup
{
public:
	DoomRay() : Powerup() { Start(); };
	~DoomRay() {};

	void Start() override {
		timer = 10;

		rayanimator = new Animator(false);
		rayanimator->AddAnimation("doomray", "assets/sprites/doomray.png");
		rayanimator->SetAnimation("doomray");

		endanimator = new Animator(false);
		endanimator->AddAnimation("doomrayhit", "assets/sprites/doomrayhit.png");
		endanimator->SetAnimation("doomrayhit");
	}

	void Use() override {
		for (int i = 0; i < RNG::GetRange(1, 4); i++)
			rayanimator->current->Update();
		endanimator->current->Update();
		Vector2 direction = user->GetLastDirection();
		Vector2 extents = Playfield::GetInstance()->GetExtents();
		Vector2 position = Playfield::GetInstance()->position;

		Line line(position, position + direction * 1000);

		position = user->position;
		endPoint = position;
		if (!Line::Intersects(line, *Playfield::GetInstance()->GetWallArea(), endPoint, true)) {
			Vector2 playfield_pos = Playfield::GetInstance()->position;
			if (direction.x == 0 && direction.y != 0) {
				// Vertical direction (up/down)
				float boundary_y = playfield_pos.y + (direction.y > 0 ? extents.y : -extents.y);
				endPoint = Vector2(position.x, boundary_y);
			}
			else if (direction.y == 0 && direction.x != 0) {
				// Horizontal direction (left/right)
				float boundary_x = playfield_pos.x + (direction.x > 0 ? extents.x : -extents.x);
				endPoint = Vector2(boundary_x, position.y);
			}
		}

		float girth = direction.x == 0 ? user->sprite.getGlobalBounds().width : user->sprite.getGlobalBounds().height;
		rect = Rect(position, endPoint);
		direction = Vector2(direction.y, direction.x);
		rect.Eat(position - direction * girth);
		rect.Eat(position + direction * girth);

		for (auto& enemy : SceneManager::GetInstance()->GetActiveScene()->GetObjectsWithTag(1)) {
			bool isBoss = false;
			for (auto& tag : enemy->subTags) {
				if (tag == 2) {
					isBoss = true;
					break;
				}
			}
			if (isBoss) continue;
			if (rect.Contains(enemy->position, true)) {
				Score::Add(enemy->score);
				enemy->Destroy(enemy);
				break;
			}
		}
	}

	void Draw(sf::RenderTarget& target) override {
		Vector2 direction = user->GetLastDirection();
		Vector2 position = user->position;

		Vector2 frameSize = rayanimator->current->frameSize;
		sf::Sprite* currentSprite = &rayanimator->current->sprite;

		// Calculate unit steps in the direction
		float step = frameSize.y;
		float totalLength = (direction.x != 0) ? std::abs(rect.GetSize().x) : std::abs(rect.GetSize().y);
		int tiles = static_cast<int>(totalLength / step - 1);

		// Set sprite origin based on direction
		if (direction.x > 0) // Right
			currentSprite->setOrigin(currentSprite->getGlobalBounds().height / 2, currentSprite->getGlobalBounds().width);
		else if (direction.x < 0) // Left
			currentSprite->setOrigin(currentSprite->getGlobalBounds().height / 2, currentSprite->getGlobalBounds().width);
		else if (direction.y > 0) // Down
			currentSprite->setOrigin(currentSprite->getGlobalBounds().width / 2, currentSprite->getGlobalBounds().height);
		else if (direction.y < 0) // Up
			currentSprite->setOrigin(currentSprite->getGlobalBounds().width / 2, currentSprite->getGlobalBounds().height);

		currentSprite->setRotation(Vector2::Degrees(direction));

		// Draw the ray in tiles
		for (int i = 0; i <= tiles; ++i) {
			Vector2 offset = direction * (i * step);
			currentSprite->setPosition(position.x + offset.x, position.y + offset.y);
			rayanimator->current->Update();
			target.draw(*currentSprite);
		}

		// Draw the ray endpoint
		currentSprite = &endanimator->current->sprite;
		if (direction.x > 0) // Right
			currentSprite->setOrigin(currentSprite->getGlobalBounds().width / 2, 0);
		else if (direction.x < 0) // Left
			currentSprite->setOrigin(currentSprite->getGlobalBounds().width / 2, 0);
		else if (direction.y > 0) // Down
			currentSprite->setOrigin(currentSprite->getGlobalBounds().width / 2, 0);
		else if (direction.y < 0) // Up
			currentSprite->setOrigin(currentSprite->getGlobalBounds().width / 2, 0);

		currentSprite->setRotation(Vector2::Degrees(direction));
		currentSprite->setPosition(endPoint.x, endPoint.y);
		target.draw(*currentSprite);
	}

	
private:
	Animator* rayanimator;
	Animator* endanimator;
	Rect rect;
	Vector2 endPoint;
};

