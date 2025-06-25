#include "Splitter.h"

#include "Debug.hpp"

void Splitter::Awake()
{
    score = 50;
    if (!texture.loadFromFile("assets/sprites/splitter.png")) {
        std::cerr << "Error loading splitter texture\n";
        return;
    }
    sprite.setTexture(texture);
    sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);

    direction = Vector2(1, 0); // Default direction right
    //direction = Vector2(0, 1); // Default direction down
	sprite.setRotation(Vector2::Degrees(direction));

    sf::FloatRect bounds = sprite.getGlobalBounds();
    Rect rect(Vector2(bounds.left, bounds.top), Vector2(bounds.left + bounds.width, bounds.top + bounds.height));
    hitbox = new Hitbox(position, { rect }, 0, 1);
    mirrorHitbox = new Hitbox(position, { rect }, 0, 1);
}

void Splitter::Start()
{
    speed = 1;
}

void Splitter::Update()
{
    if (inactive) return;

    frames++;

    float amplitude = 100.0f;
    float frequency = 2.5f;
    t = sin(frames * Time::GetInstance()->GetDeltaTime() * frequency) * amplitude;

    // Move along direction, apply sine wave perpendicular
    if (t >= threshhold) position = position + direction * speed;

    // Calculate perpendicular vector for sine wave
    Vector2 perp(-direction.y, direction.x);
    Vector2 pos = position + perp * (t > threshhold ? t : threshhold);

    // Update hitboxes
    if (hitbox) {
        hitbox->SetCenter(pos);
        hitbox->CalculateHitbox();
    }
    if (mirrorHitbox) {
        mirrorHitbox->SetCenter(pos.RotateAround(position, 180));
        mirrorHitbox->CalculateHitbox();
    }

    // Check if the splitter is out of bounds
    std::vector<Vector2> points;
    Playfield* playfield = Playfield::GetInstance();
    bool OOB = false;
    if (hitbox) {
        for (Rect& rect : hitbox->GetRects()) {
            points.push_back(rect.min);
            points.push_back(rect.max);
            points.push_back(rect.GetOtherMin());
            points.push_back(rect.GetOtherMax());
        }
    }
    if (mirrorHitbox) {
        for (Rect& rect : mirrorHitbox->GetRects()) {
            points.push_back(rect.min);
            points.push_back(rect.max);
            points.push_back(rect.GetOtherMin());
            points.push_back(rect.GetOtherMax());
        }
    }
    for (Vector2& point : points) {
        if (!playfield->IsInBounds(point, false)) {
            OOB = true;
            break;
        }
    }
    if (OOB) {
        direction = direction * -1; // Reverse direction
    }
}

void Splitter::Draw(sf::RenderTarget& target)
{
    if (clipped) return;

    Vector2 perp(-direction.y, direction.x);
    Vector2 pos = position + perp * (t > threshhold ? t : threshhold);

    if (direction.x > 0 || direction.y > 0) {
        sprite.setScale(1, 1);
        sprite.setPosition(pos.x, pos.y);
        target.draw(sprite);

        Vector2 mirror = pos.RotateAround(position, 180);
        sprite.setScale(-1, -1);
        sprite.setPosition(mirror.x, mirror.y);
        target.draw(sprite);
    }
    else {
        sprite.setScale(-1, -1);
        sprite.setPosition(pos.x, pos.y);
        target.draw(sprite);

        Vector2 mirror = pos.RotateAround(position, 180);
        sprite.setScale(1, 1);
        sprite.setPosition(mirror.x, mirror.y);
        target.draw(sprite);
    }

    // Debug draw the hitbox if it exists
    if (hitbox) {
        for (const Rect& rect : hitbox->GetRects()) {
            Debug::DrawWireRect(rect, sf::Color::Green);
        }
    }
    if (mirrorHitbox) {
        for (const Rect& rect : mirrorHitbox->GetRects()) {
            Debug::DrawWireRect(rect, sf::Color::Green);
        }
    }
}
