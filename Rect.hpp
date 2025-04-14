#pragma once
#include "Vector2.hpp"

namespace shape {
	class Rect
	{
	public:
		inline Rect() : min(0, 0), max(0, 0) {}
		inline Rect(Vector2 min, Vector2 max) : min(min), max(max) {}
		inline Rect(std::vector<Vector2> points) {
			if (points.size() < 2) return;
			min = points[0];
			max = points[0];
			for (size_t i = 1; i < points.size(); i++)
				Eat(points[i]);
		}

		Vector2 min;
		Vector2 max;

		/// <summary>
		/// Does the point lie within the rectangle?
		/// </summary>
		inline bool Contains(const Vector2& point) {
			if (point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y) {
				return true;
			}
			return false;
		}

		/// <summary>
		/// Does the rectangle intersect with another rectangle?
		/// </summary>
		inline bool Intersects(const Rect& other) {
			return !(min.x > other.max.x || max.x < other.min.x || min.y < other.max.y || max.y > other.min.y);
		}

		/// <summary>
		/// Expands the rectangle to include the point.
		/// </summary>
		inline void Eat(const Vector2& point) {
			if (point.x < min.x) min.x = point.x;
			if (point.y > min.y) min.y = point.y;
			if (point.x > max.x) max.x = point.x;
			if (point.y < max.y) max.y = point.y;
		}

		/// <summary>
		/// Returns the closest point on the rectangle to the given point. If the point is inside the rectangle, it returns the point itself.
		/// </summary>
		inline Vector2 ClosestPoint(const Vector2& point) {
			Vector2 closestPoint;
			closestPoint.x = std::max(min.x, std::min(point.x, max.x));
			closestPoint.y = std::max(min.y, std::min(point.y, max.y));
			return closestPoint;
		}

		/// <summary>
		/// Returns the closest point on the perimeter of the rectangle to the given point.
		/// </summary>
		inline Vector2 ClosestPerimeterPoint(const Vector2& point) {
			Vector2 closestPoint = ClosestPoint(point);
			if (Contains(point)) {
				if (point.x < min.x) closestPoint.x = min.x;
				if (point.y > min.y) closestPoint.y = min.y;
				if (point.x > max.x) closestPoint.x = max.x;
				if (point.y < max.y) closestPoint.y = max.y;
			}
			return closestPoint;
		}

		inline float SurfaceArea() {
			Vector2 dis = Vector2::AxisDistance(min, max);
			return dis.x * dis.y;
		}

		inline float Perimeter() {
			Vector2 dis = Vector2::AxisDistance(min, max);
			return dis.x * 2 + dis.y * 2;
		}

		inline Vector2 GetCenter() {
			return Vector2((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f);
		}

		inline Vector2 GetSize() {
			return Vector2(max.x - min.x, max.y - min.y);
		}

		inline Vector2 GetExtents() {
			return GetSize() * 0.5f;
		}

		/// <summary>
		/// Shorthand for writing Vector2(rect.max.x, rect.min.y)
		/// </summary>
		inline Vector2 GetOtherMin() {
			return Vector2(max.x, min.y);
		}

		/// <summary>
		/// Shorthand for writing Vector2(rect.min.x, rect.max.y)
		/// </summary>
		inline Vector2 GetOtherMax() {
			return Vector2(min.x, max.y);
		}
	};
}