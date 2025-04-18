#pragma once
#include <algorithm>
#include <random>
#include "Object.h"
#include "Input.hpp"
#include "Time.h"
#include "RNG.hpp"
#include "Line.hpp"
#include "Score.hpp"

using shape::Line;

class SnakeSegment
{
public:
    SnakeSegment(Object* parent, int index, Vector2 position, std::vector<Vector2>* path, sf::Sprite* sprite) :
        parent(parent), index(index), position(position), path(path), sprite(sprite) {
		sprite->setOrigin(sprite->getLocalBounds().width / 2, sprite->getLocalBounds().height / 2);
    };
    ~SnakeSegment() {};

    Object* parent;
	Vector2 position;
	std::vector<Vector2>* path;
    int index;
    sf::Sprite* sprite;

    inline void Update() {
        if (path->size() > index)
            position = path->at(index);
    };

    inline void Draw(sf::RenderTarget& target) {
        sprite->setPosition(position.x, position.y);
        sprite->setRotation(Vector2::Degrees(Vector2::Direction(position, path->at(index + 1))));
        target.draw(*sprite);
    };
};

class Snake : public Object
{
public:
    Snake() : Object("Snake") { Awake(); };
    Snake(Vector2 position) : Object("Snake", position) { Awake(); };
    ~Snake() {};

    void Awake() override;
    void Start() override;
    void Update() override;
    void Draw(sf::RenderTarget& target) override;

    nlohmann::json ToJson() const override {
        return {
            {"type", "Snake"},
			{"name", name},
			{"tag", tag},
			{"subTags", subTags},
            {"length", length},
            {"segmentOffset", segmentOffset},
            {"position", {position.x, position.y}}
        };
    }

    void FromJson(const nlohmann::json& json) override {  
       name = json["name"];
	   tag = json["tag"];
       subTags = json["subTags"].get<std::vector<int>>();
       length = json["length"];
	   segmentOffset = json["segmentOffset"];
       position = Vector2(json["position"][0], json["position"][1]);
    }

    bool IsTouching(const Object& other) const override;

private:
    int speed = 4;
    int angle = 0;
    int rotationSpeed = 2;
    float turnAroundTimer = 0;

    int length;
    int segmentOffset = 5;
    std::vector<Vector2> path;
    std::vector<SnakeSegment> segments;

    sf::Texture segmentTexture;
    sf::Sprite segmentSprite;
};