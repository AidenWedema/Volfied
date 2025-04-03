#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include "json.hpp"
#include "Vector2.hpp"

class Object
{
public:
	Object() {};
	Object(std::string name) : name(name) {};
	Object(Vector2 position) : position(position) {};
	Object(std::string name, Vector2 position) : name(name), position(position) {};
	virtual ~Object() {};

	std::string name;
	Vector2 position;

	sf::Texture texture;
	sf::Sprite sprite;

	/// <summary>
	/// Awake is called when the object is created.
	/// </summary>
	virtual void Awake() {};

	/// <summary>
	/// Start is called at the first frame of the object being loaded.
	/// </summary>
	virtual void Start() {};

	/// <summary>
	/// Update is called every frame.
	/// </summary>
	virtual void Update() {};

	/// <summary>
	/// Draw is called every frame after Update to draw the objects sprite.
	/// </summary>
	/// <param name="target">The sf::RenderTarget to draw on.</param>
	virtual void Draw(sf::RenderTarget& target) {
		sprite.setPosition(position.x, position.y);
		target.draw(sprite);
	}

	virtual nlohmann::json ToJson() const {
		return {
			{"type", "Object"},
			{"name", name},
			{"position", {position.x, position.y}}
		};
	}

	virtual void FromJson(const nlohmann::json& json) {
		name = json["name"];
		position = Vector2(json["position"][0], json["position"][1]);
	}
};