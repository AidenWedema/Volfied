#pragma once
#include "Object.h"
#include "Playfield.h"
#include "RNG.hpp"
#include "Line.hpp"
#include "Score.hpp"
#include "Bullet.h"

class GunShip : public Object
{
public:
	GunShip() : Object("GunShip") { Awake(); };
	GunShip(Vector2 position) : Object("GunShip", position) { Awake(); };
	~GunShip() {};

	void Awake() override;
	void Start() override;
	void Update() override;

	nlohmann::json ToJson() const override {
		return {
			{"type", "GunShip"},
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
	int speed = 3;
	float direction;
	bool shooting = false;
	float shootTimer;
	Vector2 shootTime = Vector2(1, 3);
	int timesToShoot = 3;
};

