#pragma once
#include <iostream>
#include <cmath>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846 // pi
#endif

class Vector2
{
public:
	Vector2() : x(0), y(0) {}
	Vector2(float x, float y) : x(x), y(y) {}
	~Vector2() {}

	float x;
	float y;

	static Vector2 Left() { return Vector2(-1, 0); }
	static Vector2 Right() { return Vector2(1, 0); }
	static Vector2 Down() { return Vector2(0, 1); }
	static Vector2 Up() { return Vector2(0, -1); }

	Vector2 operator+(const Vector2& other) const {
		return Vector2(this->x + other.x, this->y + other.y);
	}

	Vector2 operator-(const Vector2& other) const {
		return Vector2(this->x - other.x, this->y - other.y);
	}

	Vector2 operator*(const int& other) const {
		return Vector2(x * other, y * other);
	}

	Vector2 operator*(const float& other) const {
		return Vector2(x * other, y * other);
	}

	Vector2 operator*(const Vector2& other) const {
		return Vector2(x * other.x, y * other.y);
	}

	Vector2 operator/(const int& other) const {
		return other == 0 ? Vector2() : Vector2(x / other, y / other);
	}

	Vector2 operator/(const float& other) const {
		return other == 0 ? Vector2() : Vector2(x / other, y / other);
	}

	Vector2 operator/(const Vector2& other) const {
		return Vector2(x / other.x, y / other.y);
	}

	bool operator==(const Vector2& other) const {
		return x == other.x && y == other.y;
	}

	bool operator!=(const Vector2& other) const {
		return x != other.x || y != other.y;
	}

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

	/// <summary>
	/// Returns the normalized vector.
	/// </summary>
	Vector2 Normalize() {
		float len = std::sqrt(x * x + y * y);
		if (len == 0) return Vector2(0, 0);
		return Vector2(x / len, y / len);
	}
	
	/// <summary>
	/// Returns the normalized vector.
	/// </summary>
	static Vector2 Normalize(const Vector2& v) {
		float len = std::sqrt(v.x * v.x + v.y * v.y);
		if (len == 0) return Vector2(0, 0);
		return Vector2(v.x / len, v.y / len);
	}

	/// <summary>
	/// Returns the vector rotated by the specified degrees in the clockwise direction.
	/// </summary>
	Vector2 Rotate(float degrees) {
		// Convert degrees to radians
		float radians = (float)(degrees * (M_PI / 180.0f));
		// Apply rotation formulas
		float rotatedX = x * std::cos(radians) - y * std::sin(radians);
		float rotatedY = x * std::sin(radians) + y * std::cos(radians);
		return Vector2(rotatedX, rotatedY);
	}

	/// <summary>
	/// Returns the vector rotated by the specified degrees in the clockwise direction.
	/// </summary>
	static Vector2 Rotate(const Vector2& v, float degrees) {
		// Convert degrees to radians
		float radians = (float)(degrees * (M_PI / 180.0f));
		// Apply rotation formulas
		float rotatedX = v.x * std::cos(radians) - v.y * std::sin(radians);
		float rotatedY = v.x * std::sin(radians) + v.y * std::cos(radians);
		return Vector2(rotatedX, rotatedY);
	}

	/// <summary>
	/// Rotates the vector around the center by the specified degrees in the clockwise direction.
	/// </summary>
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
		return Vector2(rotatedX + center.x, rotatedY + center.y);
	}

	/// <summary>
	/// Returns the direction from from to to.
	/// </summary>
	static Vector2 Direction(const Vector2& from, const Vector2& to) {
		Vector2 direction = to - from;
		return direction.Normalize();
	}

	/// <summary>
	/// Returns the vector as an angle in degrees.
	/// </summary>
	static float Degrees(const Vector2& v) {
		Vector2 n = Normalize(v);
		return 90 + atan2(n.y, n.x) * 180 / M_PI;
	}

	/// <summary>
	/// Moves from towards to at the specified speed.
	/// </summary>
	static Vector2 MoveTowards(const Vector2& from, const Vector2& to, float speed) {
		Vector2 direction = Direction(from, to);
		Vector2 pos = from + direction * speed;
		// Check if the new position is closer to the target
		if (Distance(from, to) < Distance(from, pos)) {
			return to; // Return the target position if overshooting
		}
		return pos;
	}

	/// <summary>
	/// The dot product of two vectors.
	/// </summary>
	static float Dot(const Vector2& v1, const Vector2& v2) {
		return v1.x * v2.x + v1.y * v2.y;
	}

	/// <summary>
	/// The cross product of two vectors.
	/// </summary>
	static float Cross(const Vector2& v1, const Vector2& v2) {
		return v1.x * v2.y - v1.y * v2.x;
	}

	/// <summary>
	/// Reflects the vector point against the normal vector at orgin.
	/// </summary>
	static Vector2 Reflect(const Vector2& point, const Vector2& origin, const Vector2& normal) {
		Vector2 dir = point - origin;
		Vector2 n = Normalize(normal);
		Vector2 reflectedDir = dir - n * (2 * Dot(dir, n));
		return origin + reflectedDir;
	}

	/// <summary>
	/// Reflects the vector point against the normal vector.
	/// </summary>
	static Vector2 Reflect(const Vector2& point, const Vector2& normal) {
		Vector2 n = Normalize(normal);
		return point - n * (2.0f * Dot(point, n));
	}

	/// <summary>
	/// Returns the angle between two vectors in degrees.
	/// </summary>
	static float Angle(const Vector2& v1, const Vector2& v2) {
		float dot = Dot(v1, v2);
		float magnitudeProduct = std::sqrt(Dot(v1, v1) * Dot(v2, v2));
		return std::acos(dot / magnitudeProduct) * (180.0f / M_PI);
	}

	/// <summary>
	/// Returns a vector from an angle in degrees.
	/// </summary>
	static Vector2 FromDegrees(const float& angle) {
		float radians = angle * (M_PI / 180.0f);
		return Vector2(std::sin(radians), -std::cos(radians));
	}

	/// <summary>
	/// Check if the line between p1 and p2 intersects with the line from p3 to p4. Puts the intersecting point in the point reference
	/// </summary>
	static bool LineIntersects(const Vector2& p1, const Vector2& p2, const Vector2& p3, const Vector2& p4, Vector2& point) {
		const float den = (p1.x - p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x - p4.x);

		if (den == 0)
			return false;

		const float t = ((p1.x - p3.x) * (p3.y - p4.y) - (p1.y - p3.y) * (p3.x - p4.x)) / den;
		const float u = -((p1.x - p2.x) * (p1.y - p3.y) - (p1.y - p2.y) * (p1.x - p3.x)) / den;

		if (t > 0 && t < 1 && u > 0 && u < 1)
		{
			point.x = p1.x + t * (p2.x - p1.x);
			point.y = p1.y + t * (p2.y - p1.y);
			return true;
		}
		return false;
	}

	/// <summary>
	/// Check if the line between p1 and p2 intersects with the line from p3 to p4.
	/// </summary>
	static bool LineIntersects(const Vector2& p1, const Vector2& p2, const Vector2& p3, const Vector2& p4) {
		Vector2 _;
		return LineIntersects(p1, p2, p3, p4, _);
	}

	/// <summary>
	/// Check if the line between p1 and p2 intersects the lineList. Puts the first intersecting point in the point reference 
	/// </summary>
	static bool LineIntersects(const Vector2& p1, const Vector2& p2, const std::vector<Vector2>& lineList, Vector2& point) {
		if (lineList.size() < 2) return false;		
		for (size_t i = 0; i < lineList.size() - 1; i++) {
			if (LineIntersects(p1, p2, lineList[i], lineList[i + 1], point))
				return true;
		}
		return false;
	}

	/// <summary>
	/// Check if the line between p1 and p2 intersects the lineList. Puts the index of the point in lineList where the line intersects in pointIndex
	/// </summary>
	static bool LineIntersects(const Vector2& p1, const Vector2& p2, const std::vector<Vector2>& lineList, int& pointIndex) {
		if (lineList.size() < 2) return false;
		for (size_t i = 0; i < lineList.size() - 1; i++) {
			if (LineIntersects(p1, p2, lineList[i], lineList[i + 1])) {
				pointIndex = i;
				return true;
			}
		}
		return false;
	}

	/// <summary>
	/// Check if the line between p1 and p2 intersects the lineList. Puts the first intersecting point in the point reference and the index of the point in lineList where the line intersects in pointIndex
	/// </summary>
	static bool LineIntersects(const Vector2& p1, const Vector2& p2, const std::vector<Vector2>& lineList, Vector2& point, int& pointIndex) {
		if (lineList.size() < 2) return false;
		for (size_t i = 0; i < lineList.size() - 1; i++) {
			if (LineIntersects(p1, p2, lineList[i], lineList[i + 1], point)) {
				pointIndex = i;
				return true;
			}
		}
		return false;
	}

	/// <summary>
	/// Check if the line between p1 and p2 intersects the lineList.
	/// </summary>
	static bool LineIntersects(const Vector2& p1, const Vector2& p2, const std::vector<Vector2>& lineList) {
		if (lineList.size() < 2) return false;
		for (size_t i = 0; i < lineList.size() - 1; i++) {
			if (LineIntersects(p1, p2, lineList[i], lineList[i + 1]))
				return true;
		}
		return false;
	}

	std::string toString() {
		return std::to_string(x) + ", " + std::to_string(y);
	}
};

// Define a custom hash function for Vector2 by specializing std::hash
namespace std {
	template <>
	struct hash<Vector2> {
		std::size_t operator()(const Vector2& v) const {
			// Combine x and y into a single hash value
			return std::hash<float>()(v.x) ^ (std::hash<float>()(v.y) << 1);
		}
	};
}