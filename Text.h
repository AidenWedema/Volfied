#pragma once
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Object.h"

namespace ui
{
	class Text : public Object
	{
	public:
		Text() {};
		~Text() {};

		inline void Draw(sf::RenderTarget& target) override
		{
			if (clipped) return;
			target.draw(text);
		}

		sf::Font font;
		sf::Text text;

		inline nlohmann::json ToJson() {
			return {
				{"type", "Text"},
				{"name", name},
				{"tag", tag},
				{"subTags", subTags},
				{"position", {position.x, position.y}},
				{"text", text.getString()},
				{"font", font.getInfo().family},
				{"size", text.getCharacterSize()},
				{"color", {text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, text.getFillColor().a}}
			};
		}

		inline void FromJson(const nlohmann::json& json) {
			name = json["name"];
			tag = json["tag"];
			subTags = json["subTags"].get<std::vector<int>>();
			SetPosition(Vector2(json["position"][0], json["position"][1]));
			if (json.contains("text")) text.setString(static_cast<std::string>(json["text"]));
			if (json.contains("font")) {
				font.loadFromFile(json["font"]);
				text.setFont(font);
			}
			if (json.contains("size")) text.setCharacterSize(json["size"]);
			if (json.contains("color")) text.setFillColor(sf::Color(json["color"][0], json["color"][1], json["color"][2], json["color"][3]));
		}

		inline void SetText(const std::string& str) { text.setString(str); }
		inline void SetFont(const std::string& path)
		{
			if (!font.loadFromFile(path))
			{
				std::cerr << "Error loading font: " << path << std::endl;
				return;
			}
			text.setFont(font);
		}
		inline void SetSize(int size) { text.setCharacterSize(size); }
		inline void SetColor(const sf::Color& color) { text.setFillColor(color); }
		inline void SetPosition(const Vector2& pos) { position = pos; text.setPosition(position.x, position.y); }
		inline void SetOrigin(const Vector2& origin) { text.setOrigin(origin.x, origin.y); }
		inline void SetStyle(sf::Text::Style style) { text.setStyle(style); }
	};
}