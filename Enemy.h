#pragma once
#include <algorithm>
#include <random>
#include "Object.h"
#include "Input.h"
#include "Time.h"
#include "RNG.h"

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
			{"position", {position.x, position.y}}
		};
	}

	void FromJson(const nlohmann::json& json) override {
		name = json["name"];
		position = Vector2(json["position"][0], json["position"][1]);
	}

	bool IsTouchingPath();

private:
	int speed = 3;
	int angle = 0;
	int rotationSpeed = 3;
	float turnAroundTimer = 0;
};

