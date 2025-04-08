#pragma once
#include "Object.h"

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

	bool IsInBounds(Vector2& point, const sf::FloatRect& rect, bool correct);
	Vector2 GetNearestPointOnEdge(const Vector2& point);

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
};