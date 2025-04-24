#pragma once
#include "Object.h"
#include "Powerup.hpp"

class Box : public Object
{
public:
	Box() : Object("Box") { Awake(); };
	Box(Vector2 position) : Object("Box", position) { Awake(); };
	~Box() {};

	void Awake() override;
	void Start() override;
	void Update() override;
	void Draw(sf::RenderTarget& target) override;

	std::vector<std::string> powers;

	nlohmann::json ToJson() const override {

		return {
			{"type", "Box"},
			{"name", name},
			{"tag", tag},
			{"subTags", subTags},
			{"position", {position.x, position.y}},
			{"powers", powers}
		};
	};

	void FromJson(const nlohmann::json& json) override {
		name = json["name"];
		tag = json["tag"];
		subTags = json["subTags"].get<std::vector<int>>();
		position = Vector2(json["position"][0], json["position"][1]);
		powers = json["powers"].get<std::vector<std::string>>();
	};

	Powerup* GetPower(std::string name);
};

