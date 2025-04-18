#include "Rect.h"
#include "Line.hpp"

bool shape::Rect::Contains(const Vector2& point, bool strict) {
    UpdateMinMax();
    if (strict && OnEdge(point)) return true;
    if (point.x > min.x && point.x < max.x && point.y > min.y && point.y < max.y) {
        return true;
    }
    return false;
}

bool shape::Rect::ContainsAny(const std::vector<Vector2>& points, bool strict) {
    for (auto& point : points) {
        if (Contains(point, strict)) return true;
    }
    return false;
}

bool shape::Rect::ContainsAll(const std::vector<Vector2>& points, bool strict) {
    for (auto& point : points) {
        if (!Contains(point, strict)) return false;
    }
    return true;
}

bool shape::Rect::OnEdge(const Vector2& point) {
    UpdateMinMax();
    if (point.x == min.x || point.x == max.x) {
        if (point.y >= min.y && point.y <= max.y) return true;
    }
    if (point.y == min.y || point.y == max.y) {
        if (point.x >= min.x && point.x <= max.x) return true;
    }
    return false;
}

bool shape::Rect::Intersects(Rect& other) {
    UpdateMinMax();
    other.UpdateMinMax();
    return !(max.x <= other.min.x || min.x >= other.max.x ||
        max.y <= other.min.y || min.y >= other.max.y);
}

std::vector<Vector2> shape::Rect::IntersectPoints(Rect& other) {
    UpdateMinMax();
    other.UpdateMinMax();

    std::vector<Vector2> points;

    // Create lines from both rectangles
    std::vector<shape::Line> linesA = {
        shape::Line(min, GetOtherMin()),
        shape::Line(GetOtherMin(), max),
        shape::Line(max, GetOtherMax()),
        shape::Line(GetOtherMax(), min)
    };

    std::vector<shape::Line> linesB = {
        shape::Line(other.min, other.GetOtherMin()),
        shape::Line(other.GetOtherMin(), other.max),
        shape::Line(other.max, other.GetOtherMax()),
        shape::Line(other.GetOtherMax(), other.min)
    };

    for (const auto& lineA : linesA) {
        for (const auto& lineB : linesB) {
            Vector2 intersection;
            if (shape::Line::Intersects(lineA, lineB, intersection)) {
                points.push_back(intersection);
            }
        }
    }

    return points;
}

void shape::Rect::Eat(const Vector2& point) {
    UpdateMinMax();
    if (point.x < min.x) min.x = point.x;
    if (point.y < min.y) min.y = point.y;
    if (point.x > max.x) max.x = point.x;
    if (point.y > max.y) max.y = point.y;
    UpdateMinMax();
}

std::vector<shape::Rect> shape::Rect::Slice(Rect& other) {
    UpdateMinMax();
	other.UpdateMinMax();
    std::vector<Rect> result;

    if (!Intersects(other)) {
        result.push_back(*this);
        return result;
    }

    // Cut top
    if (other.max.y < max.y) {
        result.emplace_back(Vector2(min.x, other.max.y), Vector2(max.x, max.y));
    }
    // Cut bottom
    if (other.min.y > min.y) {
        result.emplace_back(Vector2(min.x, min.y), Vector2(max.x, other.min.y));
    }
    // Cut left
    if (other.min.x > min.x) {
        float top = std::min(max.y, other.max.y);
        float bottom = std::max(min.y, other.min.y);
        result.emplace_back(Vector2(min.x, bottom), Vector2(other.min.x, top));
    }
    // Cut right
    if (other.max.x < max.x) {
        float top = std::min(max.y, other.max.y);
        float bottom = std::max(min.y, other.min.y);
        result.emplace_back(Vector2(other.max.x, bottom), Vector2(max.x, top));
    }

    return result;
}

Vector2 shape::Rect::ClosestPoint(const Vector2& point) {
    UpdateMinMax();
    Vector2 closestPoint;
    closestPoint.x = std::max(min.x, std::min(point.x, max.x));
    closestPoint.y = std::max(min.y, std::min(point.y, max.y));
    return closestPoint;
}

Vector2 shape::Rect::ClosestPerimeterPoint(const Vector2& point) {
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

Vector2 shape::Rect::ClosestCorner(const Vector2& point) {
    UpdateMinMax();
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

float shape::Rect::SurfaceArea() {
    UpdateMinMax();
    Vector2 dis = Vector2::AxisDistance(min, max);
    return dis.x * dis.y;
}

float shape::Rect::Perimeter() {
    UpdateMinMax();
    Vector2 dis = Vector2::AxisDistance(min, max);
    return dis.x * 2 + dis.y * 2;
}

Vector2 shape::Rect::GetCenter() {
    UpdateMinMax();
    return Vector2((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f);
}

Vector2 shape::Rect::GetSize() {
    UpdateMinMax();
    return Vector2(max.x - min.x, max.y - min.y);
}

Vector2 shape::Rect::GetExtents() {
    return GetSize() * 0.5f;
}

Vector2 shape::Rect::GetOtherMin() {
    UpdateMinMax();
    return Vector2(max.x, min.y);
}

Vector2 shape::Rect::GetOtherMax() {
    UpdateMinMax();
    return Vector2(min.x, max.y);
}

void shape::Rect::UpdateMinMax() {
    Vector2 tempMin(min.x, min.y);
    Vector2 tempMax(max.x, max.y);
    min = Vector2(std::min(tempMin.x, tempMax.x), std::min(tempMin.y, tempMax.y));
    max = Vector2(std::max(tempMin.x, tempMax.x), std::max(tempMin.y, tempMax.y));
}