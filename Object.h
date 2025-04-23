#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include <fstream>
#include "json.hpp"
#include "Vector2.hpp"

class Object
{
public:
	inline Object() : score(0), tag(-1), shouldDie(false) { clipped = false; inactive = false; };
	inline Object(std::string name) : name(name), score(0), tag(-1), shouldDie(false) { clipped = false; inactive = false; };
	inline Object(Vector2 position) : score(0), position(position), tag(-1), shouldDie(false) { clipped = false; inactive = false; };
	inline Object(std::string name, Vector2 position) : name(name), score(0), position(position), tag(-1), shouldDie(false) { clipped = false; inactive = false; };
	inline virtual ~Object() {};

	std::string name;				// The name of the object
	int score;						// The amount of score the object is worth
	int tag;						// The main tag of the object
	std::vector<int> subTags;		// The sub tags of the object
	Vector2 position;				// The position of the object
	bool shouldDie;					// If the object should be destroyed
	bool clipped;					// If the object is clipped it is not being drawn
	bool inactive;					// If the object is inactive it is not being updated

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
		if (clipped) return;

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


	/// <summary>
	/// Marks the object for destruction. The object is deleted at the start of the next frame.
	/// </summary>
	static void Destroy(Object* obj);

	/// <summary>
	/// Instantiates a prefab object from a JSON file.
	/// </summary>
	Object* Instantiate(std::string path);
};