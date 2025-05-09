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

    // Get playfield boundary
    Vector2 extents = GetExtents();
    Vector2 topLeft = Vector2(position.x - extents.x, position.y - extents.y);
    Vector2 bottomRight = Vector2(position.x + extents.x, position.y + extents.y);

    // Find the boss position
    Vector2 bossPos = position;

    // Create two complete polygon areas - one for left side and one for right side
    std::vector<Rect> leftAreas;
    std::vector<Rect> rightAreas;

    // ----------------------------------------
    // Special case: if only 2 points (single line)
    // ----------------------------------------
    if (points.size() == 2) {
        HandleSingleLine(points, topLeft, bottomRight, leftAreas, rightAreas);
    }
    // ----------------------------------------
    // General case: complex shape with many points
    // ----------------------------------------
    else {
        // Close the path if it's not already closed
        if (points.front() != points.back()) {
            points.push_back(points.front());
        }

        // Create the left and right regions
        CreateLeftRightRegions(points, topLeft, bottomRight, leftAreas, rightAreas);
    }

    // Determine which side contains the boss and fill the opposite side
    bool bossInLeft = false;
    for (auto& area : leftAreas) {
        if (area.Contains(bossPos)) {
            bossInLeft = true;
            break;
        }
    }

    std::vector<Rect> areasToFill;
    if (bossInLeft) {
        areasToFill = rightAreas;
    }
    else {
        areasToFill = leftAreas;
    }

    // Also detect and fill any isolated areas (holes)
    std::vector<Rect> isolatedAreas = DetectIsolatedAreas(topLeft, bottomRight, bossPos, areasToFill);
    areasToFill.insert(areasToFill.end(), isolatedAreas.begin(), isolatedAreas.end());

    AddWalls(areasToFill);
}

void Playfield::HandleSingleLine(const std::vector<Vector2>& points,
    const Vector2& topLeft,
    const Vector2& bottomRight,
    std::vector<Rect>& leftAreas,
    std::vector<Rect>& rightAreas)
{
    Vector2 direction = Vector2::Direction(points[0], points[1]);

    // For horizontal line
    if (direction.y == 0 && direction.x != 0) {
        // Create top and bottom regions
        std::vector<Vector2> topPoints = {
            Vector2(std::min(points[0].x, points[1].x), topLeft.y),
            Vector2(std::max(points[0].x, points[1].x), topLeft.y),
            Vector2(std::max(points[0].x, points[1].x), points[0].y),
            Vector2(std::min(points[0].x, points[1].x), points[0].y)
        };

        std::vector<Vector2> bottomPoints = {
            Vector2(std::min(points[0].x, points[1].x), points[0].y),
            Vector2(std::max(points[0].x, points[1].x), points[0].y),
            Vector2(std::max(points[0].x, points[1].x), bottomRight.y),
            Vector2(std::min(points[0].x, points[1].x), bottomRight.y)
        };

        if (direction.x > 0) {
            leftAreas.push_back(Rect(topPoints));
            rightAreas.push_back(Rect(bottomPoints));
        }
        else {
            rightAreas.push_back(Rect(topPoints));
            leftAreas.push_back(Rect(bottomPoints));
        }
    }
    // For vertical line
    else if (direction.x == 0 && direction.y != 0) {
        // Create left and right regions
        std::vector<Vector2> leftPoints = {
            Vector2(topLeft.x, std::min(points[0].y, points[1].y)),
            Vector2(points[0].x, std::min(points[0].y, points[1].y)),
            Vector2(points[0].x, std::max(points[0].y, points[1].y)),
            Vector2(topLeft.x, std::max(points[0].y, points[1].y))
        };

        std::vector<Vector2> rightPoints = {
            Vector2(points[0].x, std::min(points[0].y, points[1].y)),
            Vector2(bottomRight.x, std::min(points[0].y, points[1].y)),
            Vector2(bottomRight.x, std::max(points[0].y, points[1].y)),
            Vector2(points[0].x, std::max(points[0].y, points[1].y))
        };

        if (direction.y > 0) {
            rightAreas.push_back(Rect(leftPoints));
            leftAreas.push_back(Rect(rightPoints));
        }
        else {
            leftAreas.push_back(Rect(leftPoints));
            rightAreas.push_back(Rect(rightPoints));
        }
    }
}

void Playfield::CreateLeftRightRegions(const std::vector<Vector2>& points,
    const Vector2& topLeft,
    const Vector2& bottomRight,
    std::vector<Rect>& leftAreas,
    std::vector<Rect>& rightAreas)
{
    // For complex shapes, we'll use a rectangle decomposition approach
    // First, let's identify all the regions bounded by the path and the playfield boundaries

    // Create grid based on all X and Y coordinates from the points
    std::set<float> xCoords;
    std::set<float> yCoords;

    // Add all point coordinates and playfield boundaries
    xCoords.insert(topLeft.x);
    xCoords.insert(bottomRight.x);
    yCoords.insert(topLeft.y);
    yCoords.insert(bottomRight.y);

    for (const auto& point : points) {
        xCoords.insert(point.x);
        yCoords.insert(point.y);
    }

    // Convert to sorted vectors
    std::vector<float> xValues(xCoords.begin(), xCoords.end());
    std::vector<float> yValues(yCoords.begin(), yCoords.end());

    // Create all possible rectangles from grid
    std::vector<Rect> allRects;
    for (size_t x = 0; x < xValues.size() - 1; x++) {
        for (size_t y = 0; y < yValues.size() - 1; y++) {
            Vector2 min(xValues[x], yValues[y]);
            Vector2 max(xValues[x + 1], yValues[y + 1]);
            allRects.push_back(Rect(min, max));
        }
    }

    // For each rectangle, determine if it's on the left or right side of the path
    for (const auto& rect : allRects) {
        // Get center point of rectangle
        Vector2 center = (rect.min + rect.max) * 0.5f;

        // Skip if the center is on the path
        bool onPath = false;
        for (size_t i = 0; i < points.size() - 1; i++) {
            if (IsPointOnLine(center, points[i], points[i + 1])) {
                onPath = true;
                break;
            }
        }
        if (onPath) continue;

        // Determine if the center is left or right of path
        if (IsPointLeftOfPath(center, points)) {
            leftAreas.push_back(rect);
        }
        else {
            rightAreas.push_back(rect);
        }
    }

    // Optimize rectangles - merge adjacent ones
    OptimizeRectangles(leftAreas);
    OptimizeRectangles(rightAreas);
}

// Helper to check if a point is on a line segment
bool Playfield::IsPointOnLine(const Vector2& point, const Vector2& lineStart, const Vector2& lineEnd)
{
    // For axis-aligned lines only
    if (lineStart.x == lineEnd.x) {
        // Vertical line
        return (point.x == lineStart.x &&
            point.y >= std::min(lineStart.y, lineEnd.y) &&
            point.y <= std::max(lineStart.y, lineEnd.y));
    }
    else if (lineStart.y == lineEnd.y) {
        // Horizontal line
        return (point.y == lineStart.y &&
            point.x >= std::min(lineStart.x, lineEnd.x) &&
            point.x <= std::max(lineStart.x, lineEnd.x));
    }
    return false;
}

// Determine if a point is on the left side of a path using ray casting
bool Playfield::IsPointLeftOfPath(const Vector2& point, const std::vector<Vector2>& path)
{
    // Ray-casting algorithm
    int crossings = 0;

    for (size_t i = 0; i < path.size() - 1; i++) {
        Vector2 p1 = path[i];
        Vector2 p2 = path[i + 1];

        // Skip horizontal lines, they don't affect crossing count
        if (p1.y == p2.y) continue;

        // Ensure p1.y < p2.y for simplicity
        if (p1.y > p2.y) {
            std::swap(p1, p2);
        }

        // Check if the ray from point to the right crosses this segment
        if (point.y >= p1.y && point.y < p2.y) {
            // Calculate x-coordinate of intersection
            float t = (point.y - p1.y) / (p2.y - p1.y);
            float x = p1.x + t * (p2.x - p1.x);

            // If intersection is to the right of the point, count a crossing
            if (x > point.x) {
                crossings++;
            }
        }
    }

    // Even number of crossings means outside, odd means inside
    // For left/right determination, we need to know the orientation of the path
    // For a clockwise path, outside is right and inside is left
    bool isClockwise = IsPathClockwise(path);

    // If the path is clockwise, odd crossings mean left side
    // If the path is counter-clockwise, even crossings mean left side
    return (crossings % 2 == 1) == isClockwise;
}

// Determine if a polygon path is clockwise or counter-clockwise
bool Playfield::IsPathClockwise(const std::vector<Vector2>& path)
{
    // Calculate signed area
    float area = 0.0f;
    for (size_t i = 0; i < path.size() - 1; i++) {
        area += (path[i + 1].x - path[i].x) * (path[i + 1].y + path[i].y);
    }

    // Close the loop
    size_t lastIdx = path.size() - 1;
    area += (path[0].x - path[lastIdx].x) * (path[0].y + path[lastIdx].y);

    // If area is positive, path is clockwise
    return area > 0;
}

// Merge adjacent rectangles where possible to minimize the number
void Playfield::OptimizeRectangles(std::vector<Rect>& rects)
{
    bool mergeOccurred;
    do {
        mergeOccurred = false;

        for (size_t i = 0; i < rects.size(); i++) {
            for (size_t j = i + 1; j < rects.size(); j++) {
                // Try to merge rectangles i and j
                bool merged = false;
                Rect mergedRect = MergeRectanglesIfPossible(rects[i], rects[j], merged);

                if (merged) {
                    // Replace rect i with merged rect and remove rect j
                    rects[i] = mergedRect;
                    rects.erase(rects.begin() + j);
                    mergeOccurred = true;
                    break;
                }
            }
            if (mergeOccurred) break;
        }
    } while (mergeOccurred);
}

// Try to merge two rectangles if they share an edge
Rect Playfield::MergeRectanglesIfPossible(const Rect& a, const Rect& b, bool& merged)
{
    merged = false;

    // Check if rectangles can be merged horizontally
    if (a.min.y == b.min.y && a.max.y == b.max.y) {
        // Check if they share a vertical edge
        if (a.max.x == b.min.x || a.min.x == b.max.x) {
            merged = true;
            return Rect(
                Vector2(std::min(a.min.x, b.min.x), a.min.y),
                Vector2(std::max(a.max.x, b.max.x), a.max.y)
            );
        }
    }

    // Check if rectangles can be merged vertically
    if (a.min.x == b.min.x && a.max.x == b.max.x) {
        // Check if they share a horizontal edge
        if (a.max.y == b.min.y || a.min.y == b.max.y) {
            merged = true;
            return Rect(
                Vector2(a.min.x, std::min(a.min.y, b.min.y)),
                Vector2(a.max.x, std::max(a.max.y, b.max.y))
            );
        }
    }

    return a; // Return unchanged if no merge possible
}

// New method to detect isolated areas that should be filled
std::vector<Rect> Playfield::DetectIsolatedAreas(const Vector2& topLeft, const Vector2& bottomRight,
    const Vector2& bossPos, const std::vector<Rect>& filledAreas)
{
    // Create a grid representation of the playfield
    const int gridSize = 1; // The resolution of our grid - adjust as needed
    const int gridWidth = static_cast<int>((bottomRight.x - topLeft.x) / gridSize) + 1;
    const int gridHeight = static_cast<int>((bottomRight.y - topLeft.y) / gridSize) + 1;

    // Grid cells: 0 = empty, 1 = filled by walls, 2 = already checked
    std::vector<std::vector<int>> grid(gridWidth, std::vector<int>(gridHeight, 0));

    // Mark existing wall areas in the grid
    for (const auto& rect : wallArea) {
        int startX = static_cast<int>((rect.min.x - topLeft.x) / gridSize);
        int startY = static_cast<int>((rect.min.y - topLeft.y) / gridSize);
        int endX = static_cast<int>((rect.max.x - topLeft.x) / gridSize) + 1;
        int endY = static_cast<int>((rect.max.y - topLeft.y) / gridSize) + 1;

        startX = std::max(0, std::min(startX, gridWidth - 1));
        startY = std::max(0, std::min(startY, gridHeight - 1));
        endX = std::max(0, std::min(endX, gridWidth));
        endY = std::max(0, std::min(endY, gridHeight));

        for (int x = startX; x < endX; x++) {
            for (int y = startY; y < endY; y++) {
                grid[x][y] = 1;
            }
        }
    }

    // Mark new wall areas to be filled
    for (const auto& rect : filledAreas) {
        int startX = static_cast<int>((rect.min.x - topLeft.x) / gridSize);
        int startY = static_cast<int>((rect.min.y - topLeft.y) / gridSize);
        int endX = static_cast<int>((rect.max.x - topLeft.x) / gridSize) + 1;
        int endY = static_cast<int>((rect.max.y - topLeft.y) / gridSize) + 1;

        startX = std::max(0, std::min(startX, gridWidth - 1));
        startY = std::max(0, std::min(startY, gridHeight - 1));
        endX = std::max(0, std::min(endX, gridWidth));
        endY = std::max(0, std::min(endY, gridHeight));

        for (int x = startX; x < endX; x++) {
            for (int y = startY; y < endY; y++) {
                grid[x][y] = 1;
            }
        }
    }

    // Mark the boss position as checked (so we don't fill it)
    int bossX = static_cast<int>((bossPos.x - topLeft.x) / gridSize);
    int bossY = static_cast<int>((bossPos.y - topLeft.y) / gridSize);
    if (bossX >= 0 && bossX < gridWidth && bossY >= 0 && bossY < gridHeight) {
        grid[bossX][bossY] = 2; // Mark as checked
    }

    // Flood fill from boss position to mark all accessible areas
    std::queue<std::pair<int, int>> floodQueue;
    floodQueue.push({ bossX, bossY });

    while (!floodQueue.empty()) {
        auto [x, y] = floodQueue.front();
        floodQueue.pop();

        // Check and mark neighbors
        const std::vector<std::pair<int, int>> directions = {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1}
        };

        for (const auto& [dx, dy] : directions) {
            int nx = x + dx;
            int ny = y + dy;

            if (nx >= 0 && nx < gridWidth && ny >= 0 && ny < gridHeight && grid[nx][ny] == 0) {
                grid[nx][ny] = 2; // Mark as checked
                floodQueue.push({ nx, ny });
            }
        }
    }

    // All remaining empty cells (0) are unreachable - convert to rects
    std::vector<Rect> isolatedAreas;
    std::vector<std::vector<bool>> visited(gridWidth, std::vector<bool>(gridHeight, false));

    for (int x = 0; x < gridWidth; x++) {
        for (int y = 0; y < gridHeight; y++) {
            if (grid[x][y] == 0 && !visited[x][y]) {
                // Start a new isolated area
                int startX = x;
                int endX = x;
                int startY = y;
                int endY = y;

                // Expand in X direction as far as possible
                while (endX + 1 < gridWidth && grid[endX + 1][y] == 0) {
                    endX++;
                }

                // Expand in Y direction as far as possible
                bool canExpand = true;
                while (canExpand && endY + 1 < gridHeight) {
                    for (int i = startX; i <= endX; i++) {
                        if (grid[i][endY + 1] != 0) {
                            canExpand = false;
                            break;
                        }
                    }
                    if (canExpand) endY++;
                }

                // Mark this area as visited
                for (int i = startX; i <= endX; i++) {
                    for (int j = startY; j <= endY; j++) {
                        visited[i][j] = true;
                    }
                }

                // Convert grid coordinates to world coordinates
                Vector2 min(topLeft.x + (startX - 1) * gridSize, topLeft.y + (startY - 1) * gridSize);
                Vector2 max(topLeft.x + (endX + 1) * gridSize, topLeft.y + (endY + 1) * gridSize);

                // Add to isolated areas
                isolatedAreas.push_back(Rect(min, max));
            }
        }
    }

    // Optimize by merging adjacent isolated areas
    OptimizeRectangles(isolatedAreas);

    return isolatedAreas;
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
