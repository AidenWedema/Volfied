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
		inline bool Contains(const Vector2& point, bool strict=false) {
			UpdateMinMax();
			if (strict && OnEdge(point)) return true;
			if (point.x > min.x && point.x < max.x && point.y > min.y && point.y < max.y) {
				return true;
			}
			return false;
		}

		inline bool OnEdge(const Vector2& point) {
			UpdateMinMax();
			if (point.x == min.x || point.x == max.x) {
				if (point.y >= min.y && point.y <= max.y) return true;
			}
			if (point.y == min.y || point.y == max.y) {
				if (point.x >= min.x && point.x <= max.x) return true;
			}
			return false;
		}

		/// <summary>
		/// Does the rectangle intersect with another rectangle?
		/// </summary>
		inline bool Intersects(const Rect& other) {
			UpdateMinMax();
			return !(min.x >= other.max.x || max.x <= other.min.x || min.y <= other.max.y || max.y >= other.min.y);
		}

		/// <summary>
		/// Expands the rectangle to include the point.
		/// </summary>
		inline void Eat(const Vector2& point) {
			UpdateMinMax();
			if (point.x < min.x) min.x = point.x;
			if (point.y < min.y) min.y = point.y;
			if (point.x > max.x) max.x = point.x;
			if (point.y > max.y) max.y = point.y;
			UpdateMinMax();
		}

		/// <summary>
		/// Returns the closest point on the rectangle to the given point. If the point is inside the rectangle, it returns the point itself.
		/// </summary>
		inline Vector2 ClosestPoint(const Vector2& point) {
			UpdateMinMax();
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
			if (Contains(point, true)) {
				float leftDist = std::abs(point.x - min.x);
				float rightDist = std::abs(point.x - max.x);
				float topDist = std::abs(point.y - min.y);
				float bottomDist = std::abs(point.y - max.y);
				float minDist = std::min({ leftDist, rightDist, topDist, bottomDist });
				if (minDist == leftDist)
					closestPoint.x = min.x;
				else if (minDist == rightDist)
					closestPoint.x = max.x;
				else if (minDist == topDist)
					closestPoint.y = min.y;
				else if (minDist == bottomDist)
					closestPoint.y = max.y;
			}
			return closestPoint;
		}

		/// <summary>
		/// Returns the closest corner of the rectangle to the given point.
		/// </summary>
		inline Vector2 ClosestCorner(const Vector2& point) {
			Vector2 closestPoint = point;
			float tlDist = Vector2::Distance(point, Vector2(min.x, min.y));
			float trDist = Vector2::Distance(point, Vector2(max.x, min.y));
			float blDist = Vector2::Distance(point, Vector2(min.x, max.y));
			float brDist = Vector2::Distance(point, Vector2(max.x, max.y));
			float minDist = std::min({ tlDist, trDist, blDist, brDist });
			if (minDist == tlDist)
				closestPoint = Vector2(min.x, min.y);
			else if (minDist == trDist)
				closestPoint = Vector2(max.x, min.y);
			else if (minDist == blDist)
				closestPoint = Vector2(min.x, max.y);
			else if (minDist == brDist)
				closestPoint = Vector2(max.x, max.y);
			return closestPoint;
		}

		inline float SurfaceArea() {
			UpdateMinMax();
			Vector2 dis = Vector2::AxisDistance(min, max);
			return dis.x * dis.y;
		}

		inline float Perimeter() {
			UpdateMinMax();
			Vector2 dis = Vector2::AxisDistance(min, max);
			return dis.x * 2 + dis.y * 2;
		}

		inline Vector2 GetCenter() {
			UpdateMinMax();
			return Vector2((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f);
		}

		inline Vector2 GetSize() {
			UpdateMinMax();
			return Vector2(max.x - min.x, max.y - min.y);
		}

		inline Vector2 GetExtents() {
			return GetSize() * 0.5f;
		}

		/// <summary>
		/// Shorthand for writing Vector2(rect.max.x, rect.min.y)
		/// </summary>
		inline Vector2 GetOtherMin() {
			UpdateMinMax();
			return Vector2(max.x, min.y);
		}

		/// <summary>
		/// Shorthand for writing Vector2(rect.min.x, rect.max.y)
		/// </summary>
		inline Vector2 GetOtherMax() {
			UpdateMinMax();
			return Vector2(min.x, max.y);
		}
	private:
		inline void UpdateMinMax() {
			Vector2 tempMin(min.x, min.y);
			Vector2 tempMax(max.x, max.y);
			min = Vector2(std::min(tempMin.x, tempMax.x), std::min(tempMin.y, tempMax.y));
			max = Vector2(std::max(tempMin.x, tempMax.x), std::max(tempMin.y, tempMax.y));
		}
	};
}