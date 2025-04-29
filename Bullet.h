#pragma once
#include "Object.h"

class Bullet : public Object
{
public:
	Bullet() : Object("Bullet") { Awake(); };
	Bullet(Vector2 position) : Object("Bullet", position) { Awake(); };
	~Bullet() {};

	void Awake() override;
	void Start() override;
	void Update() override;

	Vector2 direction;
	float speed = 8.0f;
	int bounceCount;
};

