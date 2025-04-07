#pragma once
#include <iostream>
#include <chrono>
#include "Input.hpp"

class Time
{
public:
	~Time() {};
	inline static Time* instance = nullptr;
	inline static Time* GetInstance() {
		if (instance == nullptr)
			instance = new Time();
		return instance;
	}

	void Spin(std::chrono::steady_clock::time_point startTime) {
		auto spinstart = std::chrono::high_resolution_clock::now();
		do
		{
			if (Input::GetInstance()->GetKey("Speedup")) break;
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - startTime);
			deltaTime = duration.count() / 1000000.0f; // convert to seconds
		} while (deltaTime < minDelta);

		frameCount++;
	}

	// Getters + Setters
	void SetFps(uint16_t newFps) {
		fps = newFps;
		if (fps == 0) minDelta = 0;
		else minDelta = 1.0f / fps;
	}

	uint16_t GetFps() { return fps; }
	float GetMinDelta() { return minDelta; }
	float GetDeltaTime() { return deltaTime; }
	uint32_t GetFrameCount() { return frameCount; }

private:
	Time() {
		fps = 60;
		minDelta = 1.0f / fps;
		deltaTime = 0;
		frameCount = 0;
	};

	uint16_t fps;
	float minDelta;
	float deltaTime;
	uint32_t frameCount;
};