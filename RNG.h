#pragma once
class RNG
{
public:
	RNG() {};
	~RNG() {};

	static int seed;
	static int GetRand();
	static int GetRange(int, int);
	static float GetRange(float, float);
};