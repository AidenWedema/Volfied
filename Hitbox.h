#pragma once
#include <vector>
#include "Vector2.hpp"
#include "Rect.h"

using namespace shape;

class Hitbox
{
public:
	Hitbox() {}
	Hitbox(Vector2 center, std::vector<Rect> rects, float angle, int resolution = 10) : 
		center(center), originalRects(rects), angle(angle), resolution(resolution){
		CalculateHitbox();
	}
	~Hitbox() {}

	void CalculateHitbox();
	void CalculateHitbox(float angle);
	void CalculateHitbox(float angle, int resolution);
	bool IsTouching(Hitbox* other);

	inline void SetResolution(int res) { resolution = res; }
	inline int GetResolution() const { return resolution; }

	inline void SetAngle(float a) { angle = a; }
	inline float GetAngle() const { return angle; }

	void SetCenter(const Vector2& c);
	inline Vector2 GetCenter() const { return center; }

	inline std::vector<Rect>& GetRects() { return rects; }
	inline const std::vector<Rect>& GetOriginalRects() const { return originalRects; }

private:
	Vector2 center;						// Center of the hitbox
	std::vector<Rect> originalRects;	// Rectangles that make up the hitbox shape
	std::vector<Rect> rects;			// Rectangles that make up the hitbox
	float angle = 0;					// Angle in degrees
	int resolution;						// Maximum number of rectangles to generate per original rectangle
};

