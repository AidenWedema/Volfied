#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include "Vector2.hpp"
class Object
{
public:
	Object() {};
	Object(std::string name) : name(name) {};
	Object(Vector2 position) : position(position) {};
	Object(std::string name, Vector2 position) : name(name), position(position) {};
	~Object() {};

	std::string name;
	Vector2 position;

	sf::Texture texture;
	sf::Sprite sprite;

	virtual void Start() {};
	virtual void Update() {};
	void Draw(sf::RenderTarget target) {
		sprite.setPosition(position.x, position.y);
		target.draw(sprite);
	};
};