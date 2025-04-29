#pragma once
#include "Object.h"
#include "Player.h"

class Mimic : public Object
{
public:
	Mimic() : Object("Mimic") { Awake(); };
	Mimic(Vector2 position) : Object("Mimic", position) { Awake(); };
	~Mimic() {};

	void Awake() override;
	void Start() override;
	void Update() override;
	void Draw(sf::RenderTarget& target) override;

	nlohmann::json ToJson() const override {
		return {
			{"type", "Mimic"},
			{"name", name},
			{"tag", tag},
			{"subTags", subTags},
			{"position", {position.x, position.y}},
			{"pathDrawInterval", pathDrawInterval}
		};
	}
	void FromJson(const nlohmann::json& json) override {
		name = json["name"];
		tag = json["tag"];
		subTags = json["subTags"].get<std::vector<int>>();
		position = Vector2(json["position"][0], json["position"][1]);
		pathDrawInterval = json["pathDrawInterval"];
	}

private:
	int pathDrawInterval = 3;
	std::vector<Vector2> path;
	Vector2 playerLastPosition;
};

