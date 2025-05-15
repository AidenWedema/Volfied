#pragma once
#include "Object.h"
#include "Time.hpp"

class Flail : public Object
{
public:
	Flail() : Object("Flail") { Awake(); };
	Flail(Vector2 position) : Object("Flail", position) { Awake(); };
	~Flail() {};

	void Awake() override;
	void Start() override;
	void Update() override;
	void Draw(sf::RenderTarget& target) override;

	nlohmann::json ToJson() const override {
		return {
			{"type", "Flail"},
			{"name", name},
			{"tag", tag},
			{"subTags", subTags},
			{"position", {position.x, position.y}},
			{"speed", speed},
			{"radius", radius},
			{"counterClockwise", counterClockwise}
		};
	}

	void FromJson(const nlohmann::json& json) override {
		name = json["name"];
		tag = json["tag"];
		subTags = json["subTags"].get<std::vector<int>>();
		position = Vector2(json["position"][0], json["position"][1]);
		speed = json["speed"];
		radius = json["radius"];
		if (json.contains("counterClockwise"))
			counterClockwise = json["counterClockwise"];
	}

private:
	float speed = 5;
	float radius;
	Vector2 center;
	Vector2 playerLastPosition;
	int angle;
	bool counterClockwise;

	sf::Texture poleTexture;
	sf::Sprite poleSprite;
};
