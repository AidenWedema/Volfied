#pragma once
#include <iostream>
#include <cmath>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846 // pi
#endif

//enum Direction
//{
//	LEFT = 1,
//	RIGHT = 3,
//	DOWN = 2,
//	UP = 0
//};

class Vector2
{
public:
	Vector2() : x(0), y(0) {}
	Vector2(int x, int y) : x(x), y(y) {}
	//Vector2(Direction direction) { Vector2 dir = Vector2::DirectionToVector(direction); x = dir.x; y = dir.y; }
	~Vector2() {}

	int x;
	int y;

	static const Vector2 Left() { return Vector2(-1, 0); }
	static const Vector2 Right() { return Vector2(1, 0); }
	static const Vector2 Down() { return Vector2(0, 1); }
	static const Vector2 Up() { return Vector2(0, -1); }

	Vector2 operator+(const Vector2& other) {
		return Vector2(this->x + other.x, this->y + other.y);
	}

	Vector2 operator-(const Vector2& other) {
		return Vector2(this->x - other.x, this->y - other.y);
	}

	//Vector2 operator+(const Direction& other) {
	//	return *this + Vector2::DirectionToVector(other);
	//}

	//Vector2 operator-(const Direction& other) {
	//	return *this - Vector2::DirectionToVector(other);
	//}

	Vector2 operator*(const int& other)	{
		return Vector2(x * other, y * other);
	}

	Vector2 operator*(const Vector2& other) {
		return Vector2(x * other.x, y * other.y);
	}

	Vector2 operator/(const int& other) {
		return other == 0 ? Vector2() : Vector2(x / other, y / other);
	}

	Vector2 operator/(const Vector2& other) {
		return Vector2(x / other.x, y / other.y);
	}

	bool operator==(const Vector2& other) const {
		return x == other.x && y == other.y;
	}

	bool operator!=(const Vector2& other) const {
		return x != other.x || y != other.y;
	}

	/// <summary>
	/// Turns a direction into a vector.
	/// </summary>
	//static Vector2 DirectionToVector(Direction direction) {
	//	switch (direction)
	//	{
	//	case LEFT: return Left();
	//	case RIGHT: return Right();
	//	case DOWN: return Down();
	//	case UP: return Up();
	//	}
	//}

	/// <summary>
	/// Returns the distance between the x and y components of two vectors.
	/// </summary>
	Vector2 AxisDistance(const Vector2& other) const {
		return Vector2(x - other.x, y - other.y);
	}

	/// <summary>
	/// Returns the distance between the x and y components of two vectors.
	/// </summary>
	static Vector2 AxisDistance(const Vector2& v1, const Vector2& v2) {
		return Vector2(v1.x - v2.x, v1.y - v2.y);
	}

	/// <summary>
	/// Returns the distance between two vectors.
	/// </summary>
	float Distance(const Vector2& other) const {
		return (float)std::sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
	}

	/// <summary>
	/// Returns the distance between two vectors.
	/// </summary>
	static float Distance(const Vector2& v1, const Vector2& v2) {
		return (float)std::sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
	}

	Vector2 RotateAround(const Vector2& center, float degrees) const
	{
		// Convert degrees to radians
		float radians = (float)(degrees * (M_PI / 180.0f));

		// Translate the point to rotate around the center (make center the origin)
		float translatedX = (float)(x - center.x);
		float translatedY = (float)(y - center.y);

		// Apply rotation formulas
		float rotatedX = translatedX * std::cos(radians) - translatedY * std::sin(radians);
		float rotatedY = translatedX * std::sin(radians) + translatedY * std::cos(radians);

		// Translate the point back
		return Vector2((int8_t)(rotatedX + center.x), (int8_t)(rotatedY + center.y));
	}

	std::string toString() {
		return std::to_string(x) + ", " + std::to_string(y);
	}

	//static std::string DirectionToString(Direction direction) {
	//	switch (direction)
	//	{
	//	case LEFT:
	//		return "LEFT";
	//	case RIGHT:
	//		return "RIGHT";
	//	case DOWN:
	//		return "DOWN";
	//	case UP:
	//		return "UP";
	//	}
	//}
};

// Define a custom hash function for Vector2 by specializing std::hash
namespace std {
	template <>
	struct hash<Vector2> {
		std::size_t operator()(const Vector2& v) const {
			// Combine x and y into a single hash value
			return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
		}
	};
}