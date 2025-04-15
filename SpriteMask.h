#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Rect.hpp"

using shape::Rect;

class SpriteMask : public sf::Drawable
{
public:
    inline SpriteMask(std::string spritePath, bool visibleInsideMask = true) : visibleInsideMask(visibleInsideMask)
    {
        if (!texture.loadFromFile(spritePath)) {
            std::cerr << "Error loading texture from path: " << spritePath << std::endl;
            return;
        }
		sprite = new sf::Sprite();
        sprite->setTexture(texture);
        sprite->setOrigin(sprite->getLocalBounds().width / 2, sprite->getLocalBounds().height / 2);
        shader.loadFromFile("assets/shaders/sprite_mask.frag", sf::Shader::Fragment);
        CreateImage(Vector2(sprite->getLocalBounds().width, sprite->getLocalBounds().height));
    }

    inline void setTexture(const sf::Texture& tex)
    {
        texture = tex;
        sprite->setTexture(texture);
        CreateImage(Vector2(sprite->getLocalBounds().width, sprite->getLocalBounds().height));
    }

    inline void setRects(const std::vector<Rect>& rects)
    {
        maskRects = rects;
        CreateImage(Vector2(sprite->getLocalBounds().width, sprite->getLocalBounds().height));
    }

    inline void setVisibleInside(bool inside)
    {
        visibleInsideMask = inside;
        CreateImage(Vector2(sprite->getLocalBounds().width, sprite->getLocalBounds().height));
    }

    inline void setPosition(const Vector2& pos)
    {
        position = pos;
    }

    void CreateImage(Vector2 size);

    inline void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const override
    {
        sprite->setPosition(position.x, position.y);

        shader.setUniform("texture", texture);
        shader.setUniform("mask", maskTexture);

        sf::RenderStates modifiedStates = states;
        modifiedStates.shader = &shader;
        target.draw(*sprite, modifiedStates);
    }

    sf::Sprite* sprite;
    sf::Texture texture;

private:
    mutable sf::Shader shader;
    sf::Texture maskTexture;
    bool visibleInsideMask;
    std::vector<Rect> maskRects;
    Vector2 position;
};