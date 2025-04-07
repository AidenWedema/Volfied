#pragma once
#include <algorithm>
#include "Object.h"
#include "Input.hpp"
#include "Playfield.h"

class Player : public Object
{
public:
	Player() : Object("Player") { Awake(); };
	Player(Vector2 position) : Object("Player", position) { Awake(); };
	~Player() {};

	static Player* activePlayer;
	static Player* GetActivePlayer() {
		if (activePlayer == nullptr)
			activePlayer = new Player();
		return activePlayer;
	}

	void Awake() override;
	void Start() override;
	void Update() override;
	void Draw(sf::RenderTarget& target) override;

    nlohmann::json ToJson() const override {
		return {
			{"type", "Player"},
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

	bool DoesPathOverlap();

	std::vector<Vector2>* GetPath() { return &path; }

private:
	int speed = 3;
	Vector2 direction;
	Vector2 lastDirection;
	std::vector<Vector2> path;
};

