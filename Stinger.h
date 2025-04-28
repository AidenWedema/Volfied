#pragma once
#include "Object.h"
#include "Score.hpp"
#include "RNG.hpp"
#include "Line.hpp"

using namespace shape;
class Stinger : public Object
{
public:
	Stinger() : Object("Stinger") { Awake(); };
	Stinger(Vector2 position) : Object("Stinger", position) { Awake(); };
	~Stinger() {};

	void Awake() override;
	void Start() override;
	void Update() override;
	void Draw(sf::RenderTarget& target) override;

	nlohmann::json ToJson() const override {
		return {
			{"type", "Stinger"},
			{"name", name},
			{"tag", tag},
			{"subTags", subTags},
			{"position", {position.x, position.y}}
		};
	}

	void FromJson(const nlohmann::json& json) override {
		name = json["name"];
		tag = json["tag"];
		subTags = json["subTags"].get<std::vector<int>>();
		position = Vector2(json["position"][0], json["position"][1]);
	}
private:
	int angle = 0;
	float speed = 0;
	float maxSpeed = 5;
	float acceleration = 0.02f;
	float decceleration = 0.05f;
	float maxRotation = 3;
};