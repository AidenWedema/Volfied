#pragma once
class RNG
{
public:
	RNG() {};
	~RNG() {};

	inline static int seed = 0;

	/// <summary>
	/// Get a random number in range min (inclusive) to max (inclusive) for floats.
	/// </summary>
	inline static int GetRand() {
		seed = (seed * 5 + 1) % 8192;
		return seed;
	};

	/// <summary>
	/// Get a random number in range min inclusive to max inclusive.
	/// </summary>
	inline static int GetRange(int min, int max) {
		if (min > max) {
			int temp = max;
			max = min;
			min = temp;
		}
		int range = max - min + 1;
		int rand = GetRand() % range;
		return min + rand;
	};

	/// <summary>
	/// Get a random number in range min inclusive to max inclusive.
	/// </summary>
	inline static float GetRange(float min, float max) {
		if (min > max) {
			float temp = max;
			max = min;
			min = temp;
		}
		float randNormalized = static_cast<float>(GetRand()) / 8191.0f;
		return min + randNormalized * (max - min);
	};
};