#pragma once
#include <vector>
#include "Vector2.hpp"

namespace shape
{
	class Line
	{
	public:
		Line() : start(0, 0), end(0, 0) {}
		Line(Vector2 start, Vector2 end) : start(start), end(end) {}
		~Line() {}

		Vector2 start;
		Vector2 end;

		/// <summary>
		/// Creates a line list from a vector of points.
		/// </summary>
		inline static std::vector<Line> CreateLineList(const std::vector<Vector2>& points) {
			std::vector<Line> lines;
			if (points.size() < 2) return lines;
			for (size_t i = 0; i < points.size() - 1; i++) {
				lines.push_back(Line(points[i], points[i + 1]));
			}
			return lines;
		}

		/// <summary>
		/// Check if the line between p1 and p2 intersects with the line from p3 to p4. Puts the intersecting point in the point reference
		/// Only checks for intersections. Set strict to true to also check if the start or end of the line is on the line.
		/// </summary>
		inline static bool Intersects(const Line l1, const Line l2, Vector2& point, bool strict = false) {
			Vector2 p1 = l1.start;
			Vector2 p2 = l1.end;
			Vector2 p3 = l2.start;
			Vector2 p4 = l2.end;

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
			if (strict) {
				if (IsPointOnLine(p1, l2)) {
					point = p1;
					return true;
				}
				if (IsPointOnLine(p2, l2)) {
					point = p2;
					return true;
				}
			}
			return false;
		}

		/// <summary>
		/// Check if the line l1 intersects with the line l2.
		/// Only checks for intersections. Set strict to true to also check if the start or end of the line is on the line.
		/// </summary>
		inline static bool Intersects(const Line l1, const Line l2, bool strict = false) {
			Vector2 _;
			return Intersects(l1, l2, _, strict);
		}

		/// <summary>
		/// Check if the line intersects any line in the lineList. Puts the first intersecting point in the point reference 
		/// Only checks for intersections. Set strict to true to also check if the start or end of the line is on the lineList.
		/// </summary>
		inline static bool Intersects(const Line line, const std::vector<Line>& lineList, Vector2& point, bool strict = false) {
			if (lineList.size() < 2) return false;
			for (size_t i = 0; i < lineList.size() - 1; i++) {
				if (Intersects(line, lineList[i], point, strict))
					return true;
			}
			return false;
		}

		/// <summary>
		/// Check if the line intersects any line in the lineList. Puts the index of the point in lineList where the line intersects in pointIndex
		/// Only checks for intersections. Set strict to true to also check if the start or end of the line is on the lineList.
		/// </summary>
		inline static bool Intersects(const Line line, const std::vector<Line>& lineList, int& pointIndex, bool strict = false) {
			if (lineList.size() < 2) return false;
			for (size_t i = 0; i < lineList.size() - 1; i++) {
				if (Intersects(line, lineList[i], strict)) {
					pointIndex = i;
					return true;
				}
			}
			return false;
		}

		/// <summary>
		/// Check if the line intersects any line in the lineList. Puts the first intersecting point in the point reference and the index of the point in lineList where the line intersects in pointIndex
		/// Only checks for intersections. Set strict to true to also check if the start or end of the line is on the lineList.
		/// </summary>
		inline static bool Intersects(const Line line, const std::vector<Line>& lineList, Vector2& point, int& pointIndex, bool strict = false) {
			if (lineList.size() < 2) return false;
			for (size_t i = 0; i < lineList.size() - 1; i++) {
				if (Intersects(line, lineList[i], point, strict)) {
					pointIndex = i;
					return true;
				}
			}
			return false;
		}

		/// <summary>
		/// Check if the line intersects any line in the lineList.
		/// Only checks for intersections. Set strict to true to also check if the start or end of the line is on the lineList.
		/// </summary>
		inline static bool Intersects(const Line line, const std::vector<Line>& lineList, bool strict = false) {
			if (lineList.size() < 2) return false;
			for (size_t i = 0; i < lineList.size() - 1; i++) {
				if (Intersects(line, lineList[i], strict))
					return true;
			}
			return false;
		}

		/// <summary>
		/// Check if point is on the line.
		/// </summary>
		inline static bool IsPointOnLine(const Vector2& point, const Line line) {
			Vector2 p1 = line.start;
			Vector2 p2 = line.end;

			float cross = (point.y - p1.y) * (p2.x - p1.x) - (point.x - p1.x) * (p2.y - p1.y);
			if (std::abs(cross) > 0.00001f) return false;
			float dot = (point.x - p1.x) * (p2.x - p1.x) + (point.y - p1.y) * (p2.y - p1.y);
			if (dot < 0) return false;
			float len_sq = Vector2::Dot(p2, p2);
			if (dot > len_sq) return false;
			return true;
		}

		inline Vector2 GetDirection() {
			return Vector2::Normalize(end - start);
		}
		inline float GetLength() {
			return Vector2::Distance(start, end);
		}
	};
}
