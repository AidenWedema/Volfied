#pragma once
#include "Powerup.hpp"
#include "Player.h"
#include "Time.h"
#include "SceneManager.hpp"

class TimeFreeze : public Powerup
{
public:
	TimeFreeze() : Powerup() { Start(); };
	~TimeFreeze() {};

	void Start() override {
		timer = 10;
		for (auto& obj : SceneManager::GetInstance()->GetActiveScene()->GetObjectsWithTag(1)) {
			obj->inactive = true;
		}
	}

	void Use() override {
		timer -= Time::GetInstance()->GetDeltaTime();
		if (timer <= 0) End();
	}

	void End() override {
		for (auto& obj : SceneManager::GetInstance()->GetActiveScene()->GetObjectsWithTag(1)) {
			obj->inactive = false;
		}
	}
};
