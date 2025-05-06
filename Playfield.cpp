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
	if (bossInLeft) AddWalls(rightAreas);
	else AddWalls(leftAreas);

	Debug::DrawLineList(&sideLines, sf::Color::Green, true);
	Debug::DrawLineList(&points, sf::Color::Red, true);
}

std::vector<Vector2> Playfield::GetFullLine(std::vector<Vector2> points)
{
	std::vector<Vector2> newPoints = points;
	Vector2 extents = GetExtents();
	std::vector<Vector2> extentPoints = GetExtentPoints();

	Vector2 checkPoint = points[0];
	std::vector<Vector2> checkPoints;
	std::stack<Vector2> allCheckpoints;
	std::vector<Vector2> visitedPoints;
	std::stack<Vector2> path;
	allCheckpoints.push(checkPoint);
	bool onEdge = false;
	// Check if the starting point is on the edge
	for (auto& point : extentPoints) {
		if (checkPoint.x == point.x || checkPoint.y == point.y) {
			onEdge = true;
			break;
		}
	}
	while (!onEdge) {
		std::vector<Rect> rects;
		checkPoints.clear();
		if (!allCheckpoints.empty()) {
			checkPoint = allCheckpoints.top();
			allCheckpoints.pop();
		}
		else
			break;	// Shouldn't happen

		path.push(checkPoint);
		visitedPoints.push_back(checkPoint);

		// Check if the point is on the playfield edge
		for (auto& point : extentPoints) {
			if (checkPoint.x == point.x || checkPoint.y == point.y) {
				onEdge = true;
				break;
			}
		}

		if (onEdge) {
			std::vector<Vector2> pathPoints;
			while (!path.empty()) {
				pathPoints.push_back(path.top());
				path.pop();
			}
			// reverse the pathpoints and add to the start of newpoints
			for (int i = pathPoints.size() - 2; i >= 0; i--) {
				newPoints.insert(newPoints.begin(), pathPoints[i]);
			}
			break;
		}

		// Get all rects that the point is on the edge of
		for (auto& wall : *GetWallArea()) {
			if (wall.OnEdge(checkPoint)) {
				rects.push_back(wall);
				break;
			}
		}

		for (auto& wall : rects) {
			// Get the corners of the rect
			std::vector<Vector2> rectPoints = {
				wall.min,
				wall.GetOtherMin(),
				wall.max,
				wall.GetOtherMax()
			};
			// Check if the line from checkPoint to each edge is axis aligned
			for (auto& point : rectPoints) {
				if (std::find(visitedPoints.begin(), visitedPoints.end(), point) != visitedPoints.end() ||
					std::find(newPoints.begin(), newPoints.end(), point) != newPoints.end() ||
					Line::IsPointOnLine(point, Line::CreateLineList(newPoints))) continue;
				Vector2 dir = Line(checkPoint, point).Direction();
				// If the line is axis aligned add the point to the checkpoints
				if ((dir.x == 0) ^ (dir.y == 0)) {
					checkPoints.push_back(point);
				}
			}
		}

		// If there are no checkpoints, go back to the previous point on the path
		if (checkPoints.empty()) path.pop();
		
		for (int i = checkPoints.size() - 1; i >= 0; i--) {
			allCheckpoints.push(checkPoints[i]);
		}
	}
	checkPoint = points[points.size() - 1];
	while(!allCheckpoints.empty()) allCheckpoints.pop();
	visitedPoints.clear();
	while(!path.empty()) path.pop();
	allCheckpoints.push(checkPoint);
	checkPoints.clear();
	onEdge = false;
	// Check if the ending point is on the edge
	for (auto& point : extentPoints) {
		if (checkPoint.x == point.x || checkPoint.y == point.y) {
			onEdge = true;
			break;
		}
	}
	while (!onEdge) {
		std::vector<Rect> rects;
		checkPoints.clear();
		if (!allCheckpoints.empty()) {
			checkPoint = allCheckpoints.top();
			allCheckpoints.pop();
		}
		else
			break;	// Shouldn't happen

		path.push(checkPoint);
		visitedPoints.push_back(checkPoint);

		for (auto& point : extentPoints) {
			if (checkPoint.x == point.x || checkPoint.y == point.y) {
				onEdge = true;
				break;
			}
		}

		if (onEdge) {
			std::vector<Vector2> pathPoints;
			while (!path.empty()) {
				pathPoints.push_back(path.top());
				path.pop();
			}
			// Add the path points to the new points in reverse order
			for (int i = pathPoints.size() - 2; i >= 0; i--) {
				newPoints.push_back(pathPoints[i]);
			}
			break;
		}

		// Get all rects that the point is on the edge of
		for (auto& wall : *GetWallArea()) {
			if (wall.OnEdge(checkPoint)) {
				rects.push_back(wall);
				break;
			}
		}

		for (auto& wall : rects) {
			// Get the corners of the rect
			std::vector<Vector2> rectPoints = {
				wall.min,
				wall.GetOtherMin(),
				wall.max,
				wall.GetOtherMax()
			};
			// Check if the line from checkPoint to each edge is axis aligned
			for (auto& point : rectPoints) {
				if (std::find(visitedPoints.begin(), visitedPoints.end(), point) != visitedPoints.end() ||
					std::find(newPoints.begin(), newPoints.end(), point) != newPoints.end() ||
					Line::IsPointOnLine(point, Line::CreateLineList(newPoints))) continue;
				Vector2 dir = Line(checkPoint, point).Direction();
				// If the line is axis aligned add the point to the checkpoints
				if ((dir.x == 0) ^ (dir.y == 0)) {
					checkPoints.push_back(point);
				}
			}
		}

		// If there are no checkpoints, go back to the previous point on the path
		if (checkPoints.empty()) path.pop();

		for (int i = checkPoints.size() - 1; i >= 0; i--) {
			allCheckpoints.push(checkPoints[i]);
		}
	}
	return newPoints;
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
