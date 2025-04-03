#include "Playfield.h"
#include "Game.h"

Playfield* Playfield::instance = nullptr;
void Playfield::Awake()
{
	sf::Vector2u windowSize = Game::GetInstance()->GetWindow()->getSize();
	size = Vector2(windowSize.x, windowSize.y);
	position = size / 2;
}

void Playfield::Start()
{
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

Vector2 Playfield::GetNearestPointOnEdge(const Vector2& point)
{
	return Vector2();
}
