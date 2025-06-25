#pragma once
#include "Object.h"
class Splitter : public Object
{
public:
	Splitter() : Object("Splitter") { Awake(); };
	Splitter(Vector2 position) : Object("Splitter", position) { Awake(); };
	~Splitter() {};

	void Awake() override;
	void Start() override;
	void Update() override;
	void Draw(sf::RenderTarget& target) override;

	nlohmann::json ToJson() const override {
		return {
			{"name", name},
			{"tag", tag},
			{"subTags", subTags},
			{"position", {position.x, position.y}},
			{"type", "Splitter"}
		};
	};
	void FromJson(const nlohmann::json& json) override {
		name = json["name"];
		tag = json["tag"];
		subTags = json["subTags"].get<std::vector<int>>();
		position = Vector2(json["position"][0], json["position"][1]);
	};

private:
	Vector2 direction;
	int frames;
	float t;
	float speed;
	float threshhold = 16;
	Hitbox* mirrorHitbox = nullptr;
};

