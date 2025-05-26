#pragma once
#include "Powerup.hpp"
#include "Player.h"
#include "Time.h"
#include "Laser.h"
#include "SceneManager.hpp"
#include "DoomRay.h"

class Gun : public Powerup
{
public:
	Gun() : Powerup() { Start(); };
	~Gun() {};

	float cooldown;

	void Start() override {
		timer = 10;
	}

	void Use() override {
		cooldown -= Time::GetInstance()->GetDeltaTime();
		if (cooldown > 0) return;
		if (Input::GetInstance()->GetKey("B")) Fire();
	}

	void Upgrade() override {
		user->AddPower(new DoomRay());
		timer = 0;
	}

	void Fire() {
		cooldown = 0.5f;
		Vector2 direction = user->GetLastDirection();
		Vector2 position = user->position;
		Laser* laser = new Laser();
		laser->position = position;
		laser->direction = direction;
		SceneManager::GetInstance()->GetActiveScene()->AddObject(laser);
	}
};