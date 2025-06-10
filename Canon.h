#pragma once
#include "Object.h"
#include "Playfield.h"
#include "RNG.hpp"
#include "Line.hpp"
#include "Score.hpp"
#include "Bullet.h"

class Canon : public Object
{
public:
	Canon() : Object("Canon") { Awake(); };
	Canon(Vector2 position) : Object("Canon", position) { Awake(); };
	~Canon() {};

	void Awake() override;
	void Start() override;
	void Update() override;
	void Draw(sf::RenderTarget& target) override;

	nlohmann::json ToJson() const override {
		return {
			{"type", "Canon"},
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
	int speed = 1;
	Vector2 direction;
	float shootTimer;
	Vector2 shootTime = Vector2(0.5f, 2); // Time between shots in seconds
	float turnTimer;
	Vector2 turnTime = Vector2(0.2f, 1);
};
