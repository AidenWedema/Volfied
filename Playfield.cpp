#include "Playfield.h"
#include "Game.h"
#include "Debug.hpp"

void Playfield::Awake()
{
	if (!texture.loadFromFile("assets/sprites/mask.png")) {
		std::cerr << "Error loading player texture\n";
		return;
	}
	sprite.setTexture(texture);
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);

	animator = new Animator(false);
	animator->AddAnimation("bg", "assets/sprites/tiles/Background.png");
	animator->SetAnimation("bg");
}

void Playfield::Start()
{
	sprite.setPosition(position.x, position.y);
	mask = new SpriteMask("assets/sprites/mask.png", false);
	mask->setPosition(position);
	animator->position = position;
}

void Playfield::Update()
{
}

void Playfield::Draw(sf::RenderTarget& target)
{
	for (auto& area : wallArea) {
		Debug::DrawWireRect(area);
	}
	Debug::DrawText("Cleared: " + std::to_string(percentCleared * 100), Vector2(400, 10));
	Debug::DrawText("SCORE: " + std::to_string(Score::score * 1000), Vector2(200, 10));


	target.draw(sprite);
	animator->current->Update();
	sf::Vector2u screenSize = target.getSize();
	Vector2 size = animator->current->frameSize;

	// Loop through the screen and draw tiles
	for (int y = 0; y < static_cast<int>(screenSize.y) + 1; y += static_cast<int>(size.y)) {
		for (int x = 0; x < static_cast<int>(screenSize.x) + 1; x += static_cast<int>(size.x)) {
			animator->position = Vector2(x, y);
			animator->Draw(target);
		}
	}
	mask->draw(target);

	Vector2 extents = GetExtents();
	std::vector<Vector2> edge = {
		Vector2(position.x - extents.x, position.y - extents.y),
		Vector2(position.x + extents.x, position.y - extents.y),
		Vector2(position.x + extents.x, position.y + extents.y),
		Vector2(position.x - extents.x, position.y + extents.y)
	};
	sf::VertexArray line(sf::LineStrip, edge.size());
	for (int i = 0; i < edge.size(); i++)
	{
		line[i] = sf::Vertex(sf::Vector2f(edge[i].x, edge[i].y), sf::Color::Red);
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
				if (correct) {
					Vector2 closestPoint = area.ClosestPerimeterPoint(point);
					point.x = closestPoint.x;
					point.y = closestPoint.y;
				}
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
	std::vector<Line> edge = {
	Line(Vector2(position.x - extents.x, position.y - extents.y), Vector2(position.x + extents.x, position.y - extents.y)),
	Line(Vector2(position.x + extents.x, position.y - extents.y), Vector2(position.x + extents.x, position.y + extents.y)),
	Line(Vector2(position.x + extents.x, position.y + extents.y), Vector2(position.x - extents.x, position.y + extents.y)),
	Line(Vector2(position.x - extents.x, position.y + extents.y), Vector2(position.x - extents.x, position.y - extents.y))
	};
	Vector2 bossPos = SceneManager::GetInstance()->GetActiveScene()->GetObjectsWithSubtag(2)[0]->position;
	if (points.size() == 2) {
		Vector2 direction = Vector2::Direction(points[0], points[1]);
		Vector2 leftPoint;
		Vector2 rightPoint;
		if (direction.x == 0 && direction.y != 0) {
			leftPoint = Vector2(position.x - extents.x, points[1].y);
			rightPoint = Vector2(position.x + extents.x, points[1].y);
		}
		else if (direction.y == 0 && direction.x != 0) {
			leftPoint = Vector2(points[1].x, position.y - extents.y);
			rightPoint = Vector2(points[1].x, position.y + extents.y);
		}
		else return;
		std::vector<Vector2> leftPoints = {
			points[0],
			points[1],
			leftPoint
		};
		std::vector<Vector2> rightPoints = {
			points[0],
			points[1],
			rightPoint
		};
		Rect leftArea = Rect(leftPoints);
		Rect rightArea = Rect(rightPoints);
		if (leftArea.Contains(bossPos))
			wallArea.push_back(rightArea);
		else
			wallArea.push_back(leftArea);
		Rect fieldRect(position, position);
		for (auto& line : edge) {
			fieldRect.Eat(line.start);
			fieldRect.Eat(line.end);
		}

		float totalArea = fieldRect.SurfaceArea();
		float areaCleared = 0;
		for (auto& area : wallArea) {
			areaCleared += area.SurfaceArea();
		}
		percentCleared = areaCleared / totalArea;
		mask->setRects(wallArea);
		KillEnemiesInWall();
		return;
	}

	std::vector<Line> pointLines = Line::CreateLineList(points);
	std::vector<Rect> rightAreas;
	std::vector<Rect> leftAreas;

	Vector2 p1 = points[0];
	Vector2 p2 = points[1];
	Vector2 p3 = points[2];
	Vector2 direction;
	Vector2 nextDirection;
	Vector2 rightDirection;
	Vector2 leftDirection;
	Vector2 endPoint;
	Vector2 sidePoint;
	Line line = Line(p2, p2 + (size * direction));
	// Get the end point of the line if points ony has 3 points.
	// This makes the code work when the for loop is skipped.
	if (points.size() == 3) {
		if (!Line::Intersects(line, pointLines, endPoint))
			if (!Line::Intersects(line, rightAreas, endPoint) && !Line::Intersects(line, leftAreas, endPoint))
				if (!Line::Intersects(line, edge, endPoint, true))
					if (direction.x == 0 && direction.y != 0)
						endPoint = Vector2(p2.x, direction.y > 0 ? position.y + extents.y : position.y - extents.y);
					else if (direction.y == 0 && direction.x != 0)
						endPoint = Vector2(direction.x > 0 ? position.x + extents.x : position.x - extents.x, p2.y);
	}
	for (int i = 0; i < points.size() - 2; i++) {
		p1 = points[i];
		p2 = points[i + 1];
		p3 = points[i + 2];
		direction = Vector2::Direction(p1, p2);
		nextDirection = Vector2::Direction(p2, p3);
		rightDirection = Vector2(-direction.y, direction.x); // Add 90 degrees
		leftDirection = Vector2(direction.y, -direction.x); // Subtract 90 degrees
		line = Line(p2, p2 + (size * direction));

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

	bool bossInLeft = false;
	for (auto& area : leftAreas) {
		if (area.Contains(bossPos)) {
			bossInLeft = true;
			break;
		}
	}

	if (bossInLeft) {
		wallArea.insert(wallArea.end(), rightAreas.begin(), rightAreas.end());
	}
	else {
		wallArea.insert(wallArea.end(), leftAreas.begin(), leftAreas.end());
	}

	Rect fieldRect(position, position);
	for (auto& line : edge) {
		fieldRect.Eat(line.start);
		fieldRect.Eat(line.end);
	}

	float totalArea = fieldRect.SurfaceArea();
	float areaCleared = 0;
	for (auto& area : wallArea) {
		areaCleared = area.SurfaceArea();
	}
	percentCleared = areaCleared / totalArea;

	mask->setRects(wallArea);
	KillEnemiesInWall();
}

void Playfield::KillEnemiesInWall()
{
	for (auto& enemy : SceneManager::GetInstance()->GetActiveScene()->GetObjectsWithTag(1)) {
		for (auto& area : wallArea) {
			if (area.Contains(enemy->position)) {
				Score::Add(enemy->score);
				Destroy(enemy);
				break;
			}
		}
	}
}
