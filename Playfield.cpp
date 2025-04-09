#include "Playfield.h"
#include "Game.h"

Playfield* Playfield::instance = nullptr;
void Playfield::Awake()
{
}

void Playfield::Start()
{
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
	sf::VertexArray line(sf::LineStrip, wall.size());
	for (int i = 0; i < wall.size(); i++)
	{
		line[i] = sf::Vertex(sf::Vector2f(wall[i].x, wall[i].y), sf::Color::Red);
	}
	target.draw(line);
}

bool Playfield::IsInBounds(Vector2& point, const sf::FloatRect& rect, bool correct = false)
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
	return OOB;
}

Vector2 Playfield::GetNearestPointOnEdge(const Vector2& point)
{
	return Vector2();
}

void Playfield::FloodFill(Vector2 start)
{
	std::vector<shape::Rect> filled;				// Contains the areas that have been filled, 
	std::queue<Vector2> queue;
	queue.push(start);
	while (!queue.empty())
	{
		Vector2 current = queue.front();
		queue.pop();
		// Check if the current point is out of bounds
		// if not in bounds (current)
		// // mark current as filled
		// // add N, E, S and W positions of current to the queue
	}
}
