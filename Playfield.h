#pragma once
#include <queue>
#include <unordered_set>
#include "Object.h"
#include "Line.hpp"
#include "SpriteMask.h"
#include "Animator.hpp"

using shape::Rect;
using shape::Line;

class Playfield : public Object
{
public:
	~Playfield() {};

	static Playfield* instance;
	static Playfield* GetInstance() {
		if (instance == nullptr)
			instance = new Playfield();
		return instance;
	}

	void Awake() override;
	void Start() override;
	void Update() override;
	void Draw(sf::RenderTarget& target) override;

	nlohmann::json ToJson() const override {
		return {
			{"type", "Playfield"},
			{"name", name},
			{"position", {position.x, position.y}},
			{"size", {size.x, size.y}}
		};
	}
	void FromJson(const nlohmann::json& json) override {
		name = json["name"];
		position = Vector2(json["position"][0], json["position"][1]);
		size = Vector2(json["size"][0], json["size"][1]);
	}

	bool IsInBounds(Vector2& point, bool correct);
	void AreaFill(std::vector<Vector2> points);

	void SetSize(const Vector2& newSize) { size = newSize; }
	Vector2 GetSize() const { return size; }
	Vector2 GetExtents() const { return Vector2(size.x * 0.5f, size.y * 0.5f); }
	void AddWall(const std::vector<Vector2>& newWall) {
		for (const auto& point : newWall) {
			wall.push_back(point);
		}
	}
	std::vector<Vector2>* GetWall() { return &wall; }

private:
	Playfield() : Object("Playfield") { Awake(); };
	Vector2 size;
	std::vector<Vector2> wall;
	std::vector<Rect> wallArea;
	SpriteMask* mask;
	Animator* animator;
	float percentCleared = 0.0f;
};