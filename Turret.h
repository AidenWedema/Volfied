#pragma once
#include "Object.h"
#include "Bullet.h"
#include "RNG.hpp"

class Turret : public Object
{
public:
	Turret() : Object("Turret") { Awake(); };
	Turret(Vector2 position) : Object("Turret", position) { Awake(); };
	~Turret() {};

	void Awake() override;
	void Start() override;
	void Update() override;

	nlohmann::json ToJson() const override {
		return {
			{"type", "Turret"},
			{"name", name},
			{"tag", tag},
			{"subTags", subTags},
			{"position", {position.x, position.y}},
			{"direction", direction},
			{"directionChange", directionChange},
			{"shootTime", shootTime}
		};
	}
	void FromJson(const nlohmann::json& json) override {
		name = json["name"];
		tag = json["tag"];
		subTags = json["subTags"].get<std::vector<int>>();
		position = Vector2(json["position"][0], json["position"][1]);
		direction = json["direction"];
		directionChange = json["directionChange"];
		shootTime = json["shootTime"];
	}

private:
	float direction;
	float directionChange = 45.0f;;
	float shootTimer;
	float shootTime = 3.0f;
};
