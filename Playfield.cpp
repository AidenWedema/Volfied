#include "Playfield.h"
#include "Game.h"
#include "Debug.hpp"

Playfield* Playfield::instance = nullptr;
void Playfield::Awake()
{
}

void Playfield::Start()
{
	mask = new SpriteMask("assets/sprites/ik.png", false);
	mask->setPosition(position);

	size = size * 0.5f;
	std::vector<Vector2> wallPoints = {
		Vector2(position.x + size.x, position.y + size.y),
		Vector2(position.x + size.x, position.y - size.y),
		Vector2(position.x - size.x, position.y - size.y),
		Vector2(position.x - size.x, position.y + size.y),
		Vector2(position.x + size.x, position.y + size.y)
	};
	AddWall(wallPoints);
	size = size * 2;
}

void Playfield::Update()
{
}

void Playfield::Draw(sf::RenderTarget& target)
{
	//for (auto& area : wallArea) {
	//	Debug::DrawRect(area, sf::Color(0, 255, 0, 50));
	//	//Debug::DrawRect(area, sf::Color(RNG::GetRange(0, 255), RNG::GetRange(0, 255), RNG::GetRange(0, 255), 255));
	//}
	for (auto& area : wallArea) {
		Debug::DrawWireRect(area);
	}

	mask->draw(target);

	sf::VertexArray line(sf::LineStrip, wall.size());
	for (int i = 0; i < wall.size(); i++)
	{
		line[i] = sf::Vertex(sf::Vector2f(wall[i].x, wall[i].y), sf::Color::Red);
	}
	target.draw(line);
}

bool Playfield::IsInBounds(Vector2& point, bool correct = false)
{
	bool OOB = false;
	Vector2 extents = GetExtents();
	if (point.x < position.x - extents.x) {
		if (correct) point.x = position.x - extents.x;
		OOB = true;
	}
	if (point.y < position.y - extents.y) {
		if (correct) point.y = position.y - extents.y;
		OOB = true;
	}
	if (point.x > position.x + extents.x) {
		if (correct) point.x = position.x + extents.x;
		OOB = true;
	}
	if (point.y > position.y + extents.y) {
		if (correct) point.y = position.y + extents.y;
		OOB = true;
	}
	if (!OOB) {
		for (auto& area : wallArea) {
			if (area.Contains(point)) {
				OOB = true;
				break;
			}
		}
	}
	return !OOB;
}

void Playfield::AreaFill(std::vector<Vector2> points)
{
	if (points.size() < 2) return;
	Vector2 extents = GetExtents();
	if (points.size() == 2) {
		Vector2 direction = Vector2::Direction(points[0], points[1]);
		Vector2 rightDirection = Vector2(-direction.y, direction.x); // Add 90 degrees
		Vector2 sidePoint;
		if (direction.x == 0 && direction.y != 0)
			sidePoint = Vector2(direction.x > 0 ? position.x + extents.x : position.x - extents.x, points[1].y);
		else if (direction.y == 0 && direction.x != 0)
			sidePoint = Vector2(points[1].x, direction.y > 0 ? position.y + extents.y : position.y - extents.y);
		else return;
		std::vector<Vector2> p = {
			points[0],
			points[1],
			sidePoint
		};
		wallArea.push_back(Rect(p));
		mask->setRects(wallArea);
		return;
	}
	if (points.size() == 3) {
		Rect newArea = Rect(points);
		wallArea.push_back(newArea);
		mask->setRects(wallArea);
		return;
	}

	std::vector<Line> edge = {
		Line(Vector2(position.x - extents.x, position.y - extents.y), Vector2(position.x + extents.x, position.y - extents.y)),
		Line(Vector2(position.x + extents.x, position.y - extents.y), Vector2(position.x + extents.x, position.y + extents.y)),
		Line(Vector2(position.x + extents.x, position.y + extents.y), Vector2(position.x - extents.x, position.y + extents.y)),
		Line(Vector2(position.x - extents.x, position.y + extents.y), Vector2(position.x - extents.x, position.y - extents.y))
	};
	std::vector<Line> pointLines = Line::CreateLineList(points);
	std::vector<Rect> rightAreas;
	std::vector<Rect> leftAreas;

	Vector2 p1;
	Vector2 p2 = points[points.size() - 2];
	Vector2 p3 = points[points.size() - 1];
	Vector2 direction;
	Vector2 nextDirection;
	Vector2 rightDirection;
	Vector2 leftDirection;
	Vector2 endPoint;
	Vector2 sidePoint;
	for (int i = 0; i < points.size() - 2; i++) {
		p1 = points[i];
		p2 = points[i + 1];
		p3 = points[i + 2];
		direction = Vector2::Direction(p1, p2);
		nextDirection = Vector2::Direction(p2, p3);
		rightDirection = Vector2(-direction.y, direction.x); // Add 90 degrees
		leftDirection = Vector2(direction.y, -direction.x); // Subtract 90 degrees
		Line line = Line(p2, p2 + (size * direction));

		// Get the end point of the line
		if (!Line::Intersects(line, pointLines, endPoint))
			if (!Line::Intersects(line, rightAreas, endPoint) && !Line::Intersects(line, leftAreas, endPoint))
				if (!Line::Intersects(line, edge, endPoint, true))
					if (direction.x == 0 && direction.y != 0)
						endPoint = Vector2(p2.x, direction.y > 0 ? position.y + extents.y : position.y - extents.y);
					else if (direction.y == 0 && direction.x != 0)
						endPoint = Vector2(direction.x > 0 ? position.x + extents.x : position.x - extents.x, p2.y);
					else continue;

		// Get the side point of the line
		if (nextDirection == rightDirection) {
			line = Line(p2, p2 + (size * leftDirection));
			if (!Line::Intersects(line, pointLines, sidePoint)) {
				if (!Line::Intersects(line, leftAreas, sidePoint))
					if (!Line::Intersects(line, edge, sidePoint))
						if (direction.x == 0 && direction.y != 0)
							sidePoint = Vector2(direction.x > 0 ? position.x + extents.x : position.x - extents.x, p2.y);
						else if (direction.y == 0 && direction.x != 0)
							sidePoint = Vector2(p2.x, direction.y > 0 ? position.y + extents.y : position.y - extents.y);
						else continue;
			}
		}
		if (nextDirection == leftDirection) {
			line = Line(p2, p2 + (size * rightDirection));
			if (!Line::Intersects(line, pointLines, sidePoint)) {
				if (!Line::Intersects(line, rightAreas, sidePoint))
					if (!Line::Intersects(line, edge, sidePoint))
						if (direction.x == 0 && direction.y != 0)
							sidePoint = Vector2(direction.x > 0 ? position.x + extents.x : position.x - extents.x, p2.y);
						else if (direction.y == 0 && direction.x != 0)
							sidePoint = Vector2(p2.x, direction.y > 0 ? position.y + extents.y : position.y - extents.y);
						else continue;
			}
		}
		// Create the right area
		std::vector<Vector2> rectPoints = {
			p1,
			p2,
			endPoint,
			sidePoint
		};
		Rect newArea = Rect(rectPoints);
		if (nextDirection == rightDirection)
			leftAreas.push_back(newArea);
		if (nextDirection == leftDirection)
			rightAreas.push_back(newArea);
	}
	// Fill in the final area
	std::vector<Vector2> outPoints = {
		p2,
		p3,
		endPoint
	};
	std::vector<Vector2> inPoints = {
		p1,
		p2,
		p3,
	};
	Rect outArea = Rect(outPoints);
	Rect inArea = Rect(inPoints);
	if (nextDirection == rightDirection) {
		leftAreas.push_back(outArea);
		rightAreas.push_back(inArea);
	}
	if (nextDirection == leftDirection) {
		rightAreas.push_back(outArea);
		leftAreas.push_back(inArea);
	}

	float rightAreaTotal = 0;
	float leftAreaTotal = 0;
	for (auto& area : rightAreas) {
		rightAreaTotal += area.SurfaceArea();
	}
	for (auto& area : leftAreas) {
		leftAreaTotal += area.SurfaceArea();
	}
	if (leftAreaTotal > rightAreaTotal) {
		wallArea.insert(wallArea.end(), rightAreas.begin(), rightAreas.end());
	}
	else {
		wallArea.insert(wallArea.end(), leftAreas.begin(), leftAreas.end());
	}
	mask->setRects(wallArea);
}
