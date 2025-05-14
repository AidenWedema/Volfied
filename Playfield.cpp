#include "Playfield.h"
#include "Game.h"
#include "AudioManager.hpp"

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
	filledArea.resize(size.x + 1, std::vector<bool>(size.y + 1, false));
}

void Playfield::Update()
{
	if (inactive) return;

	if (clearTimer > 0) EndLevel();
}

void Playfield::Draw(sf::RenderTarget& target)
{
	if (clipped) return;

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
		Vector2 gridPoint = WorldToGrid(point);
		if (gridPoint.x < 0 || gridPoint.y < 0 || gridPoint.x > size.x || gridPoint.y > size.y) OOB = true;
	}
	if (!OOB) {
		Vector2 gridPoint = WorldToGrid(point);
		if(filledArea[gridPoint.x][gridPoint.y]) OOB = true;
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

	// Get the playfield bounds
	Vector2 extents = GetExtents();
	std::vector<Line> fullLine = Line::CreateLineList(points);
	Vector2 bossPos = position;

	std::vector<Rect> leftAreas;
	std::vector<Rect> rightAreas;

	FloodFill(points, bossPos);

	// Create Lines from the wallpoints
	std::vector<Line> lines;
	Line line(Vector2(-1, -1), Vector2(-1, -1));
	bool newLine = true;
	for (int x = 0; x < filledArea.size(); x++) {
		for (int y = 0; y < filledArea[x].size(); y++) {
			if (filledArea[x][y]) {
				Vector2 worldPoint = GridToWorld(Vector2(x, y));
				if (newLine) {
					line.start = worldPoint;
					line.end = worldPoint;
					newLine = false;
				}
				else if (line.start.x == worldPoint.x) {
					line.end = worldPoint;
				}
				else {
					lines.push_back(line);
					line = Line(worldPoint, worldPoint);
				}
			}
			else {
				if (!newLine) {
					lines.push_back(line);
					line = Line(Vector2(-1, -1), Vector2(-1, -1));
					newLine = true;
				}
			}
		}
	}

	// Merge lines into rects (if possible)
	std::vector<shape::Rect> rects;
	if (!lines.empty()) {
		std::sort(lines.begin(), lines.end(), [](const Line& a, const Line& b) {
			if (a.start.y == b.start.y) return a.start.x < b.start.x;
			return a.start.y < b.start.y;
			});

		for (size_t i = 0; i < lines.size();) {
			Line baseLine = lines[i];
			float yStart = baseLine.start.y;
			float yEnd = baseLine.end.y;
			float xStart = baseLine.start.x;
			float xEnd = baseLine.start.x;

			size_t j = i + 1;
			while (j < lines.size() &&
				lines[j].start.y == yStart &&
				lines[j].end.y == yEnd &&
				lines[j].start.x == lines[j].end.x &&
				lines[j].start.x == xEnd + 1 // assuming 1 unit grid spacing
				) {
				xEnd = lines[j].start.x;
				j++;
			}

			i = j;
			// Skip rects with a width or height of 0
			if (xStart == xEnd || yStart == yEnd)
				continue;

			// Construct the rect
			Vector2 topLeft(xStart - 1, yStart - 1);
			Vector2 bottomRight(xEnd + 1, yEnd + 1); // +1 to include the full grid area
			rects.emplace_back(topLeft, bottomRight);
		}
	}

	// Make the rects not go out of bounds
	for (auto& rect : rects) {
		if (rect.min.x < position.x - extents.x) rect.min.x = position.x - extents.x;
		if (rect.min.y < position.y - extents.y) rect.min.y = position.y - extents.y;
		if (rect.max.x > position.x + extents.x) rect.max.x = position.x + extents.x;
		if (rect.max.y > position.y + extents.y) rect.max.y = position.y + extents.y;
	}

	// Add the new areas to the wall area
	wallArea = rects;

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
	if (percentCleared > 1) percentCleared = 1;

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

void Playfield::FloodFill(std::vector<Vector2> points, Vector2 startPoint)
{
	std::vector<std::vector<bool>> visited = filledArea;
	std::vector<std::vector<bool>> inverse = filledArea;
	std::vector<Line> lines = Line::CreateLineList(points);

	points = GetExtentPoints();

	// mark the points and the points on the line between them as visited
	for (auto& line : Line::CreateLineList(points)) {
		Vector2 gridStart = WorldToGrid(line.start);
		Vector2 gridEnd = WorldToGrid(line.end);
		visited[gridStart.x][gridStart.y] = true;
		visited[gridEnd.x][gridEnd.y] = true;
		Vector2 dir = line.Direction();
		Vector2 point = gridStart + dir;
		while (point != gridEnd) {
			visited[point.x][point.y] = true;
			point = point + dir;
		}
	}

	// Flood fill the area
	std::queue<Vector2> queue;
	queue.push(startPoint);
	while (!queue.empty()) {
		Vector2 current = queue.front();
		Vector2 currentGrid = WorldToGrid(current);
		if (visited[currentGrid.x][currentGrid.y]) {
			queue.pop();
			continue;
		}
		visited[currentGrid.x][currentGrid.y] = true;
		queue.pop();
		if (!Line::IsPointOnLine(current, lines)) {
			// Check the 4 adjacent points
			Vector2 right = Vector2(current.x + 1, current.y);
			Vector2 left = Vector2(current.x - 1, current.y);
			Vector2 down = Vector2(current.x, current.y + 1);
			Vector2 up = Vector2(current.x, current.y - 1);
			if (IsInBounds(right)) queue.push(right);
			if (IsInBounds(left)) queue.push(left);
			if (IsInBounds(up)) queue.push(up);
			if (IsInBounds(down)) queue.push(down);
		}
	}

	// Invert the visited array to get the filled area
	for (int x = 0; x < filledArea.size(); x++) {
		for (int y = 0; y < filledArea[x].size(); y++) {
			if (inverse[x][y]) continue;
			inverse[x][y] = !visited[x][y];
		}
	}
	filledArea = inverse;
}

// Function to convert points from world coordinates to grid coordinates
Vector2 Playfield::WorldToGrid(Vector2 point)
{
	Vector2 gridPoint = point - position;
	gridPoint.x = (gridPoint.x + GetExtents().x);
	gridPoint.y = (gridPoint.y + GetExtents().y);
	return gridPoint;
}

// Function to convert points from grid coordinates to world coordinates
Vector2 Playfield::GridToWorld(Vector2 point)
{
	Vector2 worldPoint = point - GetExtents();
	worldPoint.x = (worldPoint.x + position.x);
	worldPoint.y = (worldPoint.y + position.y);
	return worldPoint;
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
