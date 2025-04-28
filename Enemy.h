#pragma once
#include <algorithm>
#include <random>
#include "Object.h"
#include "Time.h"
#include "RNG.hpp"
#include "Line.hpp"
#include "Score.hpp"

class Enemy : public Object
{
public:
	Enemy() : Object("Enemy") { Awake(); };
	Enemy(Vector2 position) : Object("Enemy", position) { Awake(); };
	~Enemy() {};

	void Awake() override;
	void Start() override;
	void Update() override;
	void Draw(sf::RenderTarget& target) override;

	nlohmann::json ToJson() const override {
		return {
			{"type", "Enemy"},
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

	void SpawnLineFollower(Vector2 start, int index, std::vector<Vector2>* path);

private:
	int speed = 3;
	int angle = 0;
	float rotationSpeed = 3.0f;
	float rotationSpeedTimer;
	float rotationSpeedChange = 0.2f;
	Vector2 rotationSpeedRange = Vector2(2.5f, 3.5f);
	float turnAroundTimer = 0;
	bool counterClockwise = false;
};

