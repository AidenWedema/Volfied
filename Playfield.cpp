#include "Playfield.h"
#include "Game.h"
#include "AudioManager.hpp"
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
	instance = this;
}

void Playfield::Start()
{
	sprite.setPosition(position.x, position.y);
	mask = new SpriteMask("assets/sprites/mask.png", false);
	mask->setPosition(position);
	animator->position = position;

	clearedText = new ui::Text();
	clearedText->SetPosition(Vector2(position.x, 550));
	clearedText->SetColor(sf::Color::White);
	clearedText->SetFont("assets/fonts/Emulogic.ttf");
	clearedText->SetSize(20);

	scoreText = new ui::Text();
	scoreText->SetPosition(Vector2(position.x, 50));
	scoreText->SetColor(sf::Color::White);
	scoreText->SetFont("assets/fonts/Emulogic.ttf");
	scoreText->SetSize(20);

	SceneManager::GetInstance()->GetActiveScene()->AddObject(clearedText);
	SceneManager::GetInstance()->GetActiveScene()->AddObject(scoreText);
}

void Playfield::Update()
{
	if (inactive) return;

	if (clearTimer > 0) EndLevel();
}

void Playfield::Draw(sf::RenderTarget& target)
{
	if (clipped) return;

	// debug draw the edges of the wallarea
	for (auto& area : wallArea) {
		Debug::DrawLine(area.min, area.GetOtherMin(), sf::Color::White);
		Debug::DrawLine(area.GetOtherMin(), area.max, sf::Color::White);
		Debug::DrawLine(area.max, area.GetOtherMax(), sf::Color::White);
		Debug::DrawLine(area.GetOtherMax(), area.min, sf::Color::White);
	}

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

	// Update the text
	clearedText->SetOrigin(Vector2(clearedText->text.getLocalBounds().width / 2, clearedText->text.getLocalBounds().height / 2));
	scoreText->SetOrigin(Vector2(scoreText->text.getLocalBounds().width / 2, scoreText->text.getLocalBounds().height / 2));
	clearedText->SetText("Cleared: " + std::to_string(percentCleared * 100) + "%");
	scoreText->SetText("Score: " + std::to_string(Score::score * 1000));

	// Draw the edges of the playfield
	Vector2 extents = GetExtents();
	std::vector<Vector2> edge = {
		Vector2(position.x - extents.x, position.y - extents.y),
		Vector2(position.x + extents.x, position.y - extents.y),
		Vector2(position.x + extents.x, position.y + extents.y),
		Vector2(position.x - extents.x, position.y + extents.y),
		Vector2(position.x - extents.x, position.y - extents.y)
	};
	sf::VertexArray line(sf::LineStrip, edge.size());
	for (int i = 0; i < edge.size(); i++)
	{
		line[i] = sf::Vertex(sf::Vector2f(edge[i].x, edge[i].y), sf::Color::Yellow);
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

bool Playfield::IsPointOnEdge(Vector2 point)
{
	std::vector<Vector2> extentPoints = GetExtentPoints();
	for (auto& p : extentPoints) {
		if (p.x == point.x || p.y == point.y) {
			return true;
		}
	}
	return false;
}

void Playfield::AreaFill(std::vector<Vector2> points)
{
	if (points.size() < 2) return;

	points = GetFullLine(points);

	Debug::ClearStaticDrawings();

	Vector2 extents = GetExtents();
	std::vector<Line> edge = {
	Line(Vector2(position.x - extents.x, position.y - extents.y), Vector2(position.x + extents.x, position.y - extents.y)),
	Line(Vector2(position.x + extents.x, position.y - extents.y), Vector2(position.x + extents.x, position.y + extents.y)),
	Line(Vector2(position.x + extents.x, position.y + extents.y), Vector2(position.x - extents.x, position.y + extents.y)),
	Line(Vector2(position.x - extents.x, position.y + extents.y), Vector2(position.x - extents.x, position.y - extents.y))
	};
	std::vector<Object*> bosses = SceneManager::GetInstance()->GetActiveScene()->GetObjectsWithSubtag(2);
	Vector2 bossPos = position;
	if (!bosses.empty()) bossPos = bosses[0]->position;

	std::vector<Rect> rightAreas;
	std::vector<Rect> leftAreas;

	// Fill in areas outside of the line
	Vector2 leftPoint;
	Vector2 rightPoint;
	Vector2 endPoint;
	Vector2 direction;

	if (points.size() == 2) {
		direction = Vector2::Direction(points[0], points[1]);
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
		leftAreas.push_back(Rect(leftPoints));
		rightAreas.push_back(Rect(rightPoints));
		bool bossInLeft = false;
		for (auto& area : leftAreas) {
			if (area.Contains(bossPos)) {
				bossInLeft = true;
				break;
			}
		}
		if (bossInLeft) AddWalls(rightAreas);
		else AddWalls(leftAreas);
		return;
	}

	std::vector<Line> pointLines = Line::CreateLineList(points);

	Vector2 p1 = points[0];
	Vector2 p2 = points[1];
	Vector2 p3 = points[2];
	Vector2 nextDirection;
	Vector2 rightDirection;
	Vector2 leftDirection;
	Vector2 sidePoint;
	Line line = Line(p2, p2 + (size * direction));
	std::vector<Line> sideLines;
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
		Debug::DrawLine(line, sf::Color::Yellow, true);

		// Get the end point of the line
		if (!Line::Intersects(line, pointLines, endPoint)) {
			bool found = false;
			float minDist = FLT_MAX;
			for (Vector2 p : points) {
				if (p == p2) continue;
				if (Line::IsPointOnLine(p, line) && Vector2::Distance(p, p2) < minDist) {
					endPoint = p;
					minDist = Vector2::Distance(p, p2);
					found = true;
				}
			}
			if (!found) {
				if (!Line::Intersects(line, rightAreas, endPoint) && !Line::Intersects(line, leftAreas, endPoint))
					if (!Line::Intersects(line, edge, endPoint, true))
						if (direction.x == 0 && direction.y != 0)
							endPoint = Vector2(p2.x, direction.y > 0 ? position.y + extents.y : position.y - extents.y);
						else if (direction.y == 0 && direction.x != 0)
							endPoint = Vector2(direction.x > 0 ? position.x + extents.x : position.x - extents.x, p2.y);
						else continue;
			}
		}

		// Get the side point of the line
		if (nextDirection == rightDirection) {
			line = Line(p2, p2 + (size * leftDirection));
			Debug::DrawLine(line, sf::Color::Green, true);
			if (!Line::Intersects(line, pointLines, sidePoint)) {
				bool found = false;
				float minDist = FLT_MAX;
				for (Vector2 p : points) {
					if (p == p2) continue;
					if (Line::IsPointOnLine(p, line) && Vector2::Distance(p, p2) < minDist) {
						sidePoint = p;
						minDist = Vector2::Distance(p, p2);
						found = true;
					}
				}
				if (!found) {
					if (!Line::Intersects(line, leftAreas, sidePoint))
						if (!Line::Intersects(line, edge, sidePoint))
							if (direction.x == 0 && direction.y != 0)
								sidePoint = Vector2(direction.x > 0 ? position.x + extents.x : position.x - extents.x, p2.y);
							else if (direction.y == 0 && direction.x != 0)
								sidePoint = Vector2(p2.x, direction.y > 0 ? position.y + extents.y : position.y - extents.y);
							else continue;
				}
			}
		}
		if (nextDirection == leftDirection) {
			line = Line(p2, p2 + (size * rightDirection));
			Debug::DrawLine(line, sf::Color::Green, true);
			if (!Line::Intersects(line, pointLines, sidePoint)) {
				bool found = false;
				float minDist = FLT_MAX;
				for (Vector2 p : points) {
					if (p == p2) continue;
					if (Line::IsPointOnLine(p, line) && Vector2::Distance(p, p2) < minDist) {
						sidePoint = p;
						minDist = Vector2::Distance(p, p2);
						found = true;
					}
				}
				if (!found) {
					if (!Line::Intersects(line, rightAreas, sidePoint))
						if (!Line::Intersects(line, edge, sidePoint))
							if (direction.x == 0 && direction.y != 0)
								sidePoint = Vector2(direction.x > 0 ? position.x + extents.x : position.x - extents.x, p2.y);
							else if (direction.y == 0 && direction.x != 0)
								sidePoint = Vector2(p2.x, direction.y > 0 ? position.y + extents.y : position.y - extents.y);
							else continue;
				}
			}
		}
		Line sideLine = Line(p2, sidePoint);
		if (Line::Intersects(line, sideLines, sidePoint)) {
			sideLine = Line(p2, sidePoint);
		}
		sideLines.push_back(sideLine);

		Debug::DrawLine(sidePoint + Vector2(-5, -5), sidePoint + Vector2(5, 5), sf::Color::Cyan, true);
		Debug::DrawLine(sidePoint + Vector2(-5, 5), sidePoint + Vector2(5, -5), sf::Color::Cyan, true);
		Debug::DrawLine(endPoint + Vector2(-5, -5), endPoint + Vector2(5, 5), sf::Color::Magenta, true);
		Debug::DrawLine(endPoint + Vector2(-5, 5), endPoint + Vector2(5, -5), sf::Color::Magenta, true);

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
		p3
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
	if (bossInLeft) AddWalls(rightAreas);
	else AddWalls(leftAreas);

	Debug::DrawLineList(&sideLines, sf::Color::Green, true);
	Debug::DrawLineList(&points, sf::Color::Red, true);
}

std::vector<Vector2> Playfield::GetFullLine(std::vector<Vector2> points)
{
	if (points.size() < 2) return points;
	if (IsPointOnEdge(points[0]) && IsPointOnEdge(points[points.size() - 1])) return points;

	std::vector<Vector2> newPoints = points;
	Vector2 extents = GetExtents();
	std::vector<Vector2> extentPoints = GetExtentPoints();

	std::vector<Vector2> allPoints = {points[0], points[points.size() - 1]};
	std::vector<Vector2*> wallPoints;
	std::vector<Vector2*> edgePoints;
	// Get all unique points from the wall area
	for (auto& area : wallArea) {
		if (std::find(allPoints.begin(), allPoints.end(), area.GetOtherMin()) == allPoints.end())
			allPoints.push_back(area.GetOtherMin());
		if (std::find(allPoints.begin(), allPoints.end(), area.min) == allPoints.end())
			allPoints.push_back(area.min);
		if (std::find(allPoints.begin(), allPoints.end(), area.GetOtherMax()) == allPoints.end())
			allPoints.push_back(area.GetOtherMax());
		if (std::find(allPoints.begin(), allPoints.end(), area.max) == allPoints.end())
			allPoints.push_back(area.max);
	}
	// move points that are on the edge to edgePoints
	for (auto& point : allPoints) {
		if (IsPointOnEdge(point)) edgePoints.push_back(&point);
		else wallPoints.push_back(&point);
	}

	// Find the shortest path from each edgepoint to the first point in points
	// The path can only be made from axis-aligned lines
	// The path can not go though any wall area or leave the wall area's edge
	// From all paths choose the one with the least amount of points
	// Repeat for the last point in points
	// The two paths are not allowed to intersect with each other or use the same point(s)
	// When a path from the edge to the first and a path to the last point in points is found, add them path to the points vector at the start and end respectively

	// Create the graph of points and their neighbors needed for Dijkstra's algorithm
	std::unordered_map<Vector2*, std::vector<Vector2*>> graph;
	for (auto& point : allPoints) {
		graph[&point] = std::vector<Vector2*>();
	}
	for (int i = 0; i < allPoints.size(); i++) {
		for (int j = i; j < allPoints.size(); j++) {
			// Skip if the points are the same
			if (i == j) continue;
			
			// Check if both points are in edgePoints, continue if so
			if (std::find(edgePoints.begin(), edgePoints.end(), &allPoints[i]) != edgePoints.end() && std::find(edgePoints.begin(), edgePoints.end(), &allPoints[j]) != edgePoints.end())
				continue;

			// Check if both points are the start and end points of points, continue if so
			if ((allPoints[i] == points[0] || allPoints[i] == points[points.size() - 1]) && (allPoints[j] == points[0] || allPoints[j] == points[points.size() - 1]))
				continue;

			// Check if the points are axis-aligned
			Vector2 dir = Vector2::Direction(allPoints[i], allPoints[j]);
			if ((dir.x == 0) ^ (dir.y == 0)) {
				// Check if the line between the two points intersects with any wall area
				Line line(allPoints[i], allPoints[j]);
				bool intersects = false;
				for (auto& area : wallArea) {
					if (Line::Intersects(line, area)) {
						intersects = true;
						break;
					}
				}
				if (!intersects) {
					// Add the points as each other's neighbors if they aren't already
					if (std::find(graph[&allPoints[i]].begin(), graph[&allPoints[i]].end(), &allPoints[j]) == graph[&allPoints[i]].end())
						graph[&allPoints[i]].push_back(&allPoints[j]);
					if (std::find(graph[&allPoints[j]].begin(), graph[&allPoints[j]].end(), &allPoints[i]) == graph[&allPoints[j]].end())
						graph[&allPoints[j]].push_back(&allPoints[i]);
				}
			}
		}
	}

	// Try to get the shortest path from each edge point to the first and last point in points
	std::unordered_map<Vector2*, std::unordered_map<Vector2*, std::vector<Vector2*>>> allPaths;
	allPaths[&points[0]] = std::unordered_map<Vector2*, std::vector<Vector2*>>();
	allPaths[&points[points.size() - 1]] = std::unordered_map<Vector2*, std::vector<Vector2*>>();

	for (auto& target : allPaths) {
		std::unordered_map<Vector2*, std::vector<Vector2*>> paths;
		for (auto& edgePoint : edgePoints) {
			paths[edgePoint] = std::vector<Vector2*>();
		}
		std::vector<Vector2*> path;
		std::stack<Vector2*> stack;
		for (auto& edgePoint : edgePoints) {
			// Clear the path and stack
			path.clear();
			stack = std::stack<Vector2*>();
			stack.push(edgePoint);
			Vector2* currentPoint = stack.top();
			while (*currentPoint != *target.first) {
				// Get the neighbors of the current point
				std::vector<Vector2*> neighbors = graph[currentPoint];
				if (neighbors.size() == 0 || stack.size() > 10000) {
					stack = std::stack<Vector2*>();
					break;
				}
				// Find the neighbor with the shortest distance to the first point in points
				float minDist = FLT_MAX;
				Vector2* closestPoint = nullptr;
				for (auto& neighbor : neighbors) {
					float dist = Vector2::Distance(*neighbor, *target.first);
					if (dist < minDist) {
						minDist = dist;
						closestPoint = neighbor;
					}
				}
				stack.push(closestPoint);
				currentPoint = closestPoint;
			}
			if (stack.empty()) continue;
			// Add the stack to the path
			std::stack<Vector2*> reverseStack;
			while (!stack.empty()) {
				reverseStack.push(stack.top());
				stack.pop();
			}
			while (!reverseStack.empty()) {
				path.push_back(reverseStack.top());
				reverseStack.pop();
			}
			// add the path to the paths
			paths[edgePoint] = path;
		}

		// Remove empty paths
		for (auto it = paths.begin(); it != paths.end();) {
			if (it->second.empty()) it = paths.erase(it);
			else it++;
		}

		target.second = paths;
	}

	//// Check for paths that intersect with each other
	//for (auto& path : allPaths) {
	//	for (auto& otherPath : allPaths) {
	//		if (path.first == otherPath.first) continue;
	//		for (auto& point : path.second) {
	//			if (std::find(otherPath.second.begin(), otherPath.second.end(), point) != otherPath.second.end()) {
	//				path.second.erase(std::remove(path.second.begin(), path.second.end(), point), path.second.end());
	//				break;
	//			}
	//		}
	//	}
	//}

	// Get the shortest path from the remaining paths
	for (auto& point : allPaths) {
		std::vector<Vector2*> shortestPath;
		float minDist = FLT_MAX;
		for (auto& path : point.second) {
			float dist = 0;
			for (int i = 0; i < path.second.size() - 1; i++) {
				dist += Vector2::Distance(*path.second[i], *path.second[i + 1]);
			}
			if (dist < minDist) {
				minDist = dist;
				shortestPath = path.second;
			}
		}

		if (*point.first == points[0]) {
			// Add the path to the start of points
			std::vector<Vector2> newPath;
			for (auto& p : shortestPath) {
				newPath.push_back(*p);
			}
			newPoints.insert(newPoints.begin(), newPath.begin(), newPath.end());
		}
		else if (*point.first == points[points.size() - 1]) {
			// Add the path to the end of points
			std::stack<Vector2> newPath;
			for (auto& p : shortestPath) {
				newPath.push(*p);
			}
			while (!newPath.empty()) {
				newPoints.push_back(newPath.top());
				newPath.pop();
			}
		}
		else continue; // Should never happen but just in case...
	}

	// remove duplicate points just in case
	std::vector<Vector2> uniquePoints;
	for (auto& point : newPoints) {
		if (std::find(uniquePoints.begin(), uniquePoints.end(), point) == uniquePoints.end())
			uniquePoints.push_back(point);
	}

	return uniquePoints;
}

void Playfield::AddWalls(std::vector<Rect> newAreas)
{
	// Add the new areas to the wall area
	wallArea.insert(wallArea.end(), newAreas.begin(), newAreas.end());

	std::vector<Rect> removedAreas;
	// Check for areas completely inside other areas and remove them
	for (size_t i = 0; i < wallArea.size(); i++) {
		for (size_t j = 0; j < wallArea.size(); j++) {
			if (i == j) continue;
			if (wallArea[i].ContainsAll({ wallArea[j].min, wallArea[j].max, wallArea[j].GetOtherMin(), wallArea[j].GetOtherMax() }))
				removedAreas.push_back(wallArea[j]);
		}
	}

	// Remove the areas that are completely inside other areas
	for (auto& area : removedAreas) {
		auto it = std::remove(wallArea.begin(), wallArea.end(), area);
		wallArea.erase(it, wallArea.end());
	}

	// Check for areas that intersect with other areas and change them to not intersect
	bool nonIntersect = false;
	while (!nonIntersect) {
		nonIntersect = true;
		std::vector<Rect> newWallArea;
		for (size_t i = 0; i < wallArea.size(); i++) {
			bool intersected = false;
			for (size_t j = i; j < wallArea.size(); j++) {
				if (i == j) continue;

				Rect& a = wallArea[i];
				Rect& b = wallArea[j];

				if (a.Intersects(b)) {
					nonIntersect = false;
					intersected = true;

					// Remove a and replace it with a sliced by b
					std::vector<Rect> split = a.Slice(b);
					newWallArea.insert(newWallArea.end(), split.begin(), split.end());
					break; // break inner loop and move to next rect
				}
			}

			if (!intersected) {
				newWallArea.push_back(wallArea[i]);
			}
		}
		wallArea = newWallArea;
	}

	// Calculate the percent cleared
	Rect fieldRect(position, position);
	for (auto& point : GetExtentPoints()) {
		fieldRect.Eat(point);
	}
	float totalArea = fieldRect.SurfaceArea();
	float areaCleared = 0;
	for (auto& area : wallArea) {
		areaCleared += area.SurfaceArea();
	}
	percentCleared = areaCleared / totalArea;

	// Update the mask
	mask->setRects(wallArea);

	// Kill any enemies in the wall
	KillEnemiesInWall();

	if (percentCleared >= 0.80f) {
		AudioManager::StopMusic();
		clearTimer = 5;
		Game::GetInstance()->SetPaused(true);
		inactive = false;
	}
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

void Playfield::EndLevel()
{
	clearTimer -= Time::GetInstance()->GetDeltaTime();
	if (clearTimer <= 0) {
		Scene* activeScene = SceneManager::GetInstance()->GetActiveScene();
		for (auto& obj : *activeScene->GetAllObjects()) {
			obj->clipped = true;
		}
		ui::Text* scoreText = dynamic_cast<ui::Text*>(Object::Instantiate("prefabs/Text"));
		scoreText->SetText("Score: " + std::to_string(Score::score * 1000));
		scoreText->SetOrigin(Vector2(scoreText->text.getLocalBounds().width / 2, scoreText->text.getLocalBounds().height / 2));
		scoreText->SetPosition(position);
		scoreText->SetColor(sf::Color::Red);

		ui::Cursor* cursor = dynamic_cast<ui::Cursor*>(Object::Instantiate("prefabs/Cursor"));
		cursor->selected = &cursor->options[0];
		cursor->selected->select->actions.push_back(3);
		cursor->selected->select->SetPosition(Vector2(position.x, position.y + 100));
		cursor->selected->select->SetText("Continue");
		cursor->selected->select->SetOrigin(Vector2(cursor->selected->select->text.getLocalBounds().width / 2, cursor->selected->select->text.getLocalBounds().height / 2));
	}

	if (Time::GetInstance()->GetFrameCount() % 20 == 0 && clearTimer > 2) {
		Vector2 extents = GetExtents();
		CutsceneObject* explosion = dynamic_cast<CutsceneObject*>(Object::Instantiate("prefabs/CutsceneObjects/explosion"));
		explosion->position = Vector2(RNG::GetRange(position.x - extents.x, position.x + extents.x), RNG::GetRange(position.y - extents.y, position.y + extents.y));
		explosion->animator.SetAnimation(0);
	}
}
