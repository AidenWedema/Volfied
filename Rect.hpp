#pragma once
#include "Vector2.hpp"

namespace shape {
	class Rect
	{
	public:
		inline Rect() : topLeft(0, 0), bottomRight(0, 0) {}
		inline Rect(Vector2 topLeft, Vector2 bottomRight) : topLeft(topLeft), bottomRight(bottomRight) {}

		Vector2 topLeft;
		Vector2 bottomRight;

		/// <summary>
		/// Does the point lie within the rectangle?
		/// </summary>
		inline bool Contains(Vector2 point) {
			if (point.x >= topLeft.x && point.x <= bottomRight.x && point.y >= topLeft.y && point.y <= bottomRight.y) {
				return true;
			}
			return false;
		}

		/// <summary>
		/// Does the rectangle intersect with another rectangle?
		/// </summary>
		inline bool Intersects(const Rect& other) {
			return !(topLeft.x > other.bottomRight.x || bottomRight.x < other.topLeft.x || topLeft.y < other.bottomRight.y || bottomRight.y > other.topLeft.y);
		}

		/// <summary>
		/// Expands the rectangle to include the point.
		/// </summary>
		inline void Eat(Vector2 point) {
			if (point.x < topLeft.x) topLeft.x = point.x;
			if (point.y > topLeft.y) topLeft.y = point.y;
			if (point.x > bottomRight.x) bottomRight.x = point.x;
			if (point.y < bottomRight.y) bottomRight.y = point.y;
		}

		inline Vector2 GetCenter() {
			return Vector2((topLeft.x + bottomRight.x) * 0.5f, (topLeft.y + bottomRight.y) * 0.5f);
		}

		inline Vector2 GetSize() {
			return Vector2(bottomRight.x - topLeft.x, bottomRight.y - topLeft.y);
		}

		inline Vector2 GetExtents() {
			return GetSize() * 0.5f;
		}

		inline Vector2 GetTopRight() {
			return Vector2(bottomRight.x, topLeft.y);
		}

		inline Vector2 GetBottomLeft() {
			return Vector2(topLeft.x, bottomRight.y);
		}
	};
}