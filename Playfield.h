#pragma once
#include <queue>
#include "Object.h"
#include "Line.hpp"
#include "SpriteMask.h"
#include "Animator.hpp"
#include "CutsceneObject.hpp"
#include "Text.h"

using shape::Rect;
using shape::Line;

class Playfield : public Object
{
friend class LevelEditor;
public:
	Playfield() : Object("Playfield") { Awake(); };
	~Playfield() {
		delete animator;
		delete mask;
	};

	inline static Playfield* instance = nullptr;
	inline static Playfield* GetInstance() {
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
	bool IsPointOnEdge(Vector2 point);
	void AreaFill(std::vector<Vector2> points);

	inline void SetSize(const Vector2& newSize) { size = newSize; }
	inline Vector2 GetSize() const { return size; }
	inline Vector2 GetExtents() const { return Vector2(size.x * 0.5f, size.y * 0.5f); }
	inline std::vector<Rect>* GetWallArea() { return &wallArea; }
	inline Vector2 GetClosestWallPoint(Vector2& pos) {
		Vector2 closestPoint = Vector2(-100000, -100000);
		for (auto& area : wallArea) {
			Vector2 point = area.ClosestPerimeterPoint(pos);
			if (Vector2::Distance(point, pos) < Vector2::Distance(closestPoint, pos))
				closestPoint = point;
		}
		return closestPoint;
	}
	inline Vector2 GetClosestEdgePoint(Vector2& pos) {
		Rect fieldRect(position, position);
		Vector2 extents = GetExtents();
		for (auto& point : GetExtentPoints()) {
			fieldRect.Eat(point);
		}
		Vector2 closestPoint = fieldRect.ClosestPerimeterPoint(pos);
		return closestPoint;
	}
	inline std::vector<Vector2> GetExtentPoints() {
		std::vector<Vector2> points;
		Vector2 extents = GetExtents();
		points.push_back(Vector2(position.x - extents.x, position.y - extents.y));
		points.push_back(Vector2(position.x + extents.x, position.y - extents.y));
		points.push_back(Vector2(position.x + extents.x, position.y + extents.y));
		points.push_back(Vector2(position.x - extents.x, position.y + extents.y));
		return points;
	}
	inline float GetPercentCleared() const { return percentCleared; }

private:
	Vector2 size;
	std::vector<Rect> wallArea;
	std::vector<std::vector<bool>> filledArea;
	SpriteMask* mask;
	Animator* animator;
	float percentCleared = 0.0f;
	float clearTimer;
	ui::Text* scoreText;
	ui::Text* clearedText;

	void FloodFill(std::vector<Vector2> points, Vector2 startPoint);
	Vector2 WorldToGrid(Vector2 point);
	Vector2 GridToWorld(Vector2 point);

	void KillEnemiesInWall();
	void EndLevel();
};