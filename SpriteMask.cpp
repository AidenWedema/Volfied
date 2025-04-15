#include "SpriteMask.h"
#include "Game.h"

void SpriteMask::CreateImage(Vector2 size)
{
    sf::Vector2u screenSize = Game::GetInstance()->GetWindow()->getSize();
    sf::Vector2f spriteSize = sprite->getLocalBounds().getSize();
    Vector2 offset;
	if (screenSize.x < spriteSize.x) offset.x = (spriteSize.x - screenSize.x) / 2;
	else offset.x = -(screenSize.x - spriteSize.x) / 2;
	if (screenSize.y < spriteSize.y) offset.y = (spriteSize.y - screenSize.y) / 2;
	else offset.y = (spriteSize.y - screenSize.y) / 2;

    sf::Image image;
    sf::Color visibleColor = visibleInsideMask ? sf::Color::White : sf::Color::Transparent;
    sf::Color invisibleColor = visibleInsideMask ? sf::Color::Transparent : sf::Color::White;

    image.create(static_cast<unsigned>(size.x), static_cast<unsigned>(size.y), invisibleColor);
    std::vector<std::vector<bool>> hasDone(static_cast<size_t>(size.x), std::vector<bool>(static_cast<size_t>(size.y), false));

    for (const auto& r : maskRects)
    {
        sf::Vector2u min(static_cast<unsigned>(std::min(std::max(0.f, r.min.x), std::max(0.f, r.max.x))), static_cast<unsigned>(std::min(std::max(0.f, r.min.y), std::max(0.f, r.max.y))));
        sf::Vector2u max(static_cast<unsigned>(std::max(r.max.x, r.min.x)), static_cast<unsigned>(std::max(r.max.y, r.min.y)));

        min.x += offset.x;
        min.y += offset.y;
        max.x += offset.x;
        max.y += offset.y;

        for (unsigned x = min.x; x < max.x && x < image.getSize().x; ++x)
        {
            for (unsigned y = min.y; y < max.y && y < image.getSize().y; ++y)
            {
                if (hasDone[x][y]) continue;
                hasDone[x][y] = true;
                image.setPixel(x, y, visibleColor);
            }
        }
    }
    maskTexture.loadFromImage(image);
}