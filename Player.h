#pragma once
#include <algorithm>
#include "Object.h"
#include "Input.hpp"
#include "Playfield.h"
#include "Powerup.hpp"

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

	std::vector<Vector2>* GetPath() { return &path; }
	Vector2 GetLastDirection() { return lastDirection; }

    inline void AddPower(Powerup* power)
    {
       // Check if the power already exists
       auto it = std::find_if(powers.begin(), powers.end(), [&](const Powerup* p) {
           return p->name == power->name;
       });
       if (it != powers.end()) {
           (*it)->Start();
           return;
       }
       power->user = this;
       powers.push_back(power);
    }

private:
	int speed = 3;
	bool isDigging = false;
	Vector2 direction;
	Vector2 lastDirection;
	std::vector<Vector2> path;
	std::vector<Powerup*> powers;

	void Walk();
	void Dig();
};

