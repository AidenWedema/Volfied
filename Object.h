#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include <fstream>
#include "json.hpp"
#include "Vector2.hpp"

class Object
{
public:
	inline Object() : tag(-1), shouldDie(false) {};
	inline Object(std::string name) : name(name), tag(-1), shouldDie(false) {};
	inline Object(Vector2 position) : position(position), tag(-1), shouldDie(false) {};
	inline Object(std::string name, Vector2 position) : name(name), position(position), tag(-1), shouldDie(false) {};
	inline virtual ~Object() {};

	std::string name;
	bool shouldDie;
	int tag;
	std::vector<int> subTags;
	Vector2 position;

	sf::Texture texture;
	sf::Sprite sprite;

	/// <summary>
	/// Awake is called when the object is created.
	/// </summary>
	inline virtual void Awake() {};

	/// <summary>
	/// Start is called at the first frame of the object being loaded.
	/// </summary>
	inline virtual void Start() {};

	/// <summary>
	/// Update is called every frame.
	/// </summary>
	inline virtual void Update() {};

	/// <summary>
	/// Draw is called every frame after Update to draw the objects sprite.
	/// </summary>
	/// <param name="target">The sf::RenderTarget to draw on.</param>
	inline virtual void Draw(sf::RenderTarget& target) {
		sprite.setPosition(position.x, position.y);
		target.draw(sprite);
	}

	inline virtual nlohmann::json ToJson() const {
		return {
			{"type", "Object"},
			{"name", name},
			{"tag", tag},
			{"subTags", subTags},
			{"position", {position.x, position.y}}
		};
	}

	inline virtual void FromJson(const nlohmann::json& json) {
		name = json["name"];
		tag = json["tag"];
		subTags = json["subTags"].get<std::vector<int>>();
		position = Vector2(json["position"][0], json["position"][1]);
	}

	inline virtual bool IsTouching(const Object& other) const {
		return sprite.getGlobalBounds().intersects(other.sprite.getGlobalBounds()) || sprite.getGlobalBounds().contains(other.position.x, other.position.y);
	}

	static void Destroy(Object* obj);
	Object* Instantiate(std::string path);
};