#pragma once
#include "Object.h"
#include "Playfield.h"
#include "RNG.hpp"
#include "Line.hpp"

using shape::Line;

class Ship : public Object
{
public:
	Ship() : Object("Ship") { Awake(); };
	Ship(Vector2 position) : Object("Ship", position) { Awake(); };
	~Ship() {};

	void Awake() override;
	void Start() override;
	void Update() override;

	nlohmann::json ToJson() const override {
		return {
			{"type", "Ship"},
			{"name", name},
			{"tag", tag},
			{"position", {position.x, position.y}}
		};
	}
	void FromJson(const nlohmann::json& json) override {
		name = json["name"];
		tag = json["tag"];
		position = Vector2(json["position"][0], json["position"][1]);
	}

private:
	int speed = 1;
	float direction;
};

