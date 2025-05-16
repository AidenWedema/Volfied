#include "Mimic.h"
#include "Player.h"
#include "Playfield.h"
#include "Time.hpp"

void Mimic::Awake()
{
	score = 50;
	if (!texture.loadFromFile("assets/sprites/mimic.png")) {
		std::cerr << "Error loading mimic texture\n";
		return;
	}
	sprite.setTexture(texture);
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
}

void Mimic::Start()
{
}

void Mimic::Update()
{
	if (inactive) return;

	Player* player = Player::GetActivePlayer();
	position = player->position.RotateAround(Playfield::GetInstance()->position, 180);

	std::vector<Vector2> playerPath = *player->GetPath();
	path.clear();
	if (playerPath.size() > 1) {
		for (int i = 0; i < playerPath.size() - 1; i++) {
			if (i % pathDrawInterval != 0) continue;
			path.push_back((playerPath[i].RotateAround(Playfield::GetInstance()->position, 180)));
			path.push_back((playerPath[i + 1].RotateAround(Playfield::GetInstance()->position, 180)));
		}
		for (int i = 0; i < path.size() - 1; i += 2) {
			Line line(path[i], path[i + 1]);
			if (playerLastPosition == player->position) continue;
			if (Line::Intersects(line, Line(playerLastPosition, player->position), true)) {
				player->Die();
				break;
			}
		}
	}

	sprite.setRotation(Vector2::Degrees(player->GetLastDirection() * Vector2(-1, -1)));
	playerLastPosition = player->position;
}

void Mimic::Draw(sf::RenderTarget& target)
{
	if (clipped) return;

	sprite.setPosition(position.x, position.y);
	target.draw(sprite);

	// Draw the path
	std::array<sf::Color, 4> colors = { sf::Color(255, 0, 0), sf::Color(255, 192, 0), sf::Color(255, 128, 0), sf::Color(225, 192, 0)};
	sf::VertexArray line(sf::Lines, path.size() + 1);
	for (int i = 0; i < path.size(); i++) {
		line[i] = sf::Vertex(sf::Vector2f(path[i].x, path[i].y), colors[(int)(Time::GetInstance()->GetFrameCount() / 7) % 4]);
	}
	line[path.size()] = sf::Vertex(sf::Vector2f(position.x, position.y), colors[(int)(Time::GetInstance()->GetFrameCount() / 7) % 4]);
	target.draw(line);
}
