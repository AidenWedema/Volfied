#pragma once
class Score
{
public:
	inline static int score = 0;

	inline static void Add(int amount) { score += amount; }
	inline static void Reset() { score = 0; }
};