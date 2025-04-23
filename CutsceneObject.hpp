#pragma once
#include <SFML/Audio.hpp>
#include "json.hpp"
#include "Object.h"
#include "Animator.hpp"

class CutsceneObject : public Object
{
public:
	CutsceneObject() : Object("CutsceneObject") { Awake(); };
	CutsceneObject(Vector2 position) : Object("CutsceneObject", position) { Awake(); };
	~CutsceneObject() {};

	void Awake() override {};
	void Start() override {};
	void Update() override {
		if (!animator.current->finished) return;
		if (destroyOnEnd) Destroy(this);
	};
	void Draw(sf::RenderTarget& target) override {
		animator.position = position;
		animator.Draw(target);
	};

	Animator animator;
	//sf::Sound sound;
	bool destroyOnEnd = false;

	nlohmann::json ToJson() const override {

		std::unordered_map<std::string, std::string> animations;
		for (auto& anim : animator.animations) {
			animations[anim.first] = anim.second.path;
		}
		return {
			{"name", name},
			{"tag", tag},
			{"subTags", subTags},
			{"position", {position.x, position.y}},
			{"animations", animations},
			{"type", "CutsceneObject"}
		};
	}

	void FromJson(const nlohmann::json& json) override {
		name = json["name"];
		tag = json["tag"];
		subTags = json["subTags"].get<std::vector<int>>();
		position = Vector2(json["position"][0], json["position"][1]);
		for (auto& anim : json["animations"].items()) {
			animator.AddAnimation(anim.key(), anim.value());
		}
	}
};
