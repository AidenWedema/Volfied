#pragma once
#include <vector>
#include "Vector2.hpp"

namespace shape {
	class Rect
	{
		friend class Rect;
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

		bool operator==(const Rect& other) const {
			return min == other.min && max == other.max;
		}

		/// <summary>
		/// Does the point lie within the rectangle?
		/// </summary>
		bool Contains(const Vector2& point, bool strict = false);

		/// <summary>
		/// Returns true if one of the points lies within the rectangle.
		/// </summary>
		bool ContainsAny(const std::vector<Vector2>& points, bool strict = false);

		/// <summary>
		/// Returns true if all of the points lie within the rectangle.
		/// </summary>
		bool ContainsAll(const std::vector<Vector2>& points, bool strict = false);

		bool OnEdge(const Vector2& point);

		/// <summary>
		/// Does the rectangle intersect with another rectangle?
		/// </summary>
		bool Intersects(Rect& other);

		/// <summary>
		/// Returns the points where the two rectangles intersect.
		/// </summary>
		std::vector<Vector2> IntersectPoints(Rect& other);

		/// <summary>
		/// Expands the rectangle to include the point.
		/// </summary>
		void Eat(const Vector2& point);

		/// <summary>
		/// 
		/// </summary>
		std::vector<shape::Rect> Slice(Rect& other);

		/// <summary>
		/// Returns the closest point on the rectangle to the given point. If the point is inside the rectangle, it returns the point itself.
		/// </summary>
		Vector2 ClosestPoint(const Vector2& point);

		/// <summary>
		/// Returns the closest point on the perimeter of the rectangle to the given point.
		/// </summary>
		Vector2 ClosestPerimeterPoint(const Vector2& point);

		/// <summary>
		/// Returns the closest corner of the rectangle to the given point.
		/// </summary>
		Vector2 ClosestCorner(const Vector2& point);

		float SurfaceArea();

		float Perimeter();

		Vector2 GetCenter();

		Vector2 GetSize();

		Vector2 GetExtents();

		/// <summary>
		/// Shorthand for writing Vector2(rect.max.x, rect.min.y)
		/// </summary>
		Vector2 GetOtherMin();

		/// <summary>
		/// Shorthand for writing Vector2(rect.min.x, rect.max.y)
		/// </summary>
		Vector2 GetOtherMax();
	private:
		void UpdateMinMax();
	};
}