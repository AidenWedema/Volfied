#pragma once
#include <vector>
#include "Object.h"

class LineFollower : public Object
{
public:
	LineFollower() : Object("LineFollower") { Awake();};
	~LineFollower() {};

	void Awake() override;
	void Start() override;
	void Update() override;

	inline nlohmann::json ToJson() const override {
		return {
			{"type", "LineFollower"},
			{"name", name},
			{"tag", tag},
			{"subTags", subTags},
			{"position", {position.x, position.y}}
		};
	}

	inline void FromJson(const nlohmann::json& json) override {
		name = json["name"];
		tag = json["tag"];
		subTags = json["subTags"].get<std::vector<int>>();
		position = Vector2(json["position"][0], json["position"][1]);
	}

	std::vector<Vector2>* path;
	float speed = 5;
	int index = 0;
};

