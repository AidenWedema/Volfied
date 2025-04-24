#pragma once
#include "Object.h"

class Laser : public Object
{
public:
	Laser() : Object("Laser") { Awake(); };
	Laser(Vector2 position) : Object("Laser", position) { Awake(); };
	~Laser() {};

	void Awake() override;
	void Start() override;
	void Update() override;
	void Draw(sf::RenderTarget& target) override;

	Vector2 direction;
	float speed = 8.0f;
};

