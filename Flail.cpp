#include "Flail.h"
#include "Player.h"

void Flail::Awake()
{
	score = 25;
	if (!texture.loadFromFile("assets/sprites/flail.png")) {
		std::cerr << "Error loading flail texture\n";
		return;
	}
	sprite.setTexture(texture);
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);

	if (!poleTexture.loadFromFile("assets/sprites/flail_pole.png")) {
		std::cerr << "Error loading flail pole texture\n";
		return;
	}
	poleSprite.setTexture(poleTexture);
	poleSprite.setOrigin(poleSprite.getLocalBounds().width / 2, poleSprite.getLocalBounds().height / 2);
}

void Flail::Start()
{
	center = position;
}

void Flail::Update()
{
	if (inactive) return;

	if (Vector2::Distance(position, center) < radius - 1) {
		position = position + Vector2::FromDegrees(angle) * 3;
		if (Vector2::Distance(position, center) >= radius - 1)
			angle = -90;
		return;
	}

	angle += counterClockwise ? -speed : speed;
	angle = angle % 360;
	if (angle < 0) angle += 360;

	Vector2 lastPos = position;
	position = (center + Vector2(1, 0) * radius).RotateAround(center, angle);

	Player* player = Player::GetActivePlayer();
	if (poleSprite.getGlobalBounds().intersects(player->GetActivePlayer()->sprite.getGlobalBounds()))
		player->Die();

	std::vector<Vector2>* p = player->GetPath();
	if (p->size() >= 1) {
		if (Line::Intersects(Line(center, lastPos), Line(player->position, player->position - player->GetLastDirection() * player->GetSpeed()), true))
			player->Die();
	}

	Vector2 point = Vector2();
	if (Line::Intersects(Line(lastPos, position), Line::CreateLineList(*p), point, true)) {
		position = point;
		counterClockwise = !counterClockwise;
		angle += counterClockwise ? -speed : speed;
		position = (center + Vector2(1, 0) * radius).RotateAround(center, angle);
	}
	Vector2 pos = position;
	if (!Playfield::GetInstance()->IsInBounds(position, true)) {
		counterClockwise = !counterClockwise;
		angle += counterClockwise ? -speed : speed;
		position = (center + Vector2(1, 0) * radius).RotateAround(center, angle);
	}
}

void Flail::Draw(sf::RenderTarget& target)
{
	if (clipped) return;

	// draw a line from the center to the flail
	sf::VertexArray line(sf::Lines, 2);
	line[0] = sf::Vertex(sf::Vector2f(center.x, center.y), sf::Color(255, 153, 0, 255));
	line[1] = sf::Vertex(sf::Vector2f(position.x, position.y), sf::Color(255, 153, 0, 255));
	target.draw(line);

	// draw the flail
	sprite.setRotation(counterClockwise ? angle : angle - 180);
	sprite.setPosition(position.x, position.y);
	target.draw(sprite);

	// draw the pole
	poleSprite.setPosition(center.x, center.y);
	target.draw(poleSprite);
}
