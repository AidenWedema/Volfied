#include "Hitbox.h"
#include "Debug.hpp"

void Hitbox::CalculateHitbox()
{
    rects.clear();

    if (originalRects.empty())
        return;

    if (resolution == 1) {
		rects = originalRects;
		return;
    }

    for (auto& origRect : originalRects)
    {
        Vector2 min = origRect.min;
        Vector2 max = origRect.max;

        float width = origRect.GetSize().x;
        float height = origRect.GetSize().y;

        // Determine dominant axis
        bool xIsLonger = width >= height;
        int resX = xIsLonger ? resolution : std::max(1, int(resolution * (width / height)));
        int resY = xIsLonger ? std::max(1, int(resolution * (height / width))) : resolution;

        float dx = width / resX;
        float dy = height / resY;

        for (int i = 0; i < resX; ++i)
        {
            float x0 = min.x + i * dx;
            float x1 = x0 + dx;

            for (int j = 0; j < resY; ++j)
            {
                float y0 = min.y + j * dy;
                float y1 = y0 + dy;

                // Define four corners
                Vector2 p00(x0, y0);
                Vector2 p10(x1, y0);
                Vector2 p01(x0, y1);
                Vector2 p11(x1, y1);

                // Rotate each corner around the hitbox center
                p00 = p00.RotateAround(center, angle);
                p10 = p10.RotateAround(center, angle);
                p01 = p01.RotateAround(center, angle);
                p11 = p11.RotateAround(center, angle);

                // Create a new rectangle from the rotated corners
                rects.emplace_back(Rect({p00, p10, p01, p11}));
            }
        }
    }
}

void Hitbox::CalculateHitbox(float angle)
{
	this->angle = angle;
	CalculateHitbox();
}

void Hitbox::CalculateHitbox(float angle, int resolution) {
    this->resolution = resolution;
    CalculateHitbox(angle);
}

bool Hitbox::IsTouching(Hitbox* other)
{
	if (other == nullptr) return false;
	if (rects.empty() || other->rects.empty()) return false;

    for (int i = 0; i < rects.size(); i++) {
		for (int j = 0; j < other->rects.size(); j++)
		{
			if (rects[i].Intersects(other->rects[j]))
				return true;
			if (rects[i].Contains(other->rects[j].min) && rects[i].Contains(other->rects[j].max))
				return true;
		}
    }
	return false;
}

void Hitbox::SetCenter(const Vector2& c)
{
	Vector2 dist = c - center;
    for (auto& rect : originalRects) {
		rect.min = rect.min + dist;
		rect.max = rect.max + dist;
    }
	center = c;
}
