#pragma once
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Object.h"
#include "Score.hpp"

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
			if (json.contains("text")) text.setString(ReplaceControlCodes(static_cast<std::string>(json["text"])));
			if (json.contains("font")) {
				font.loadFromFile(json["font"]);
				text.setFont(font);
			}
			if (json.contains("size")) text.setCharacterSize(json["size"]);
			if (json.contains("color")) text.setFillColor(sf::Color(json["color"][0], json["color"][1], json["color"][2], json["color"][3]));
		}

		inline void SetText(const std::string& str) { 
			text.setString(ReplaceControlCodes(str)); 
		}
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

	private:
		inline std::string ReplaceControlCodes(const std::string str) {
			std::vector<std::string> controlCodes = { "<score>", "<scoreboard>"};
			std::string result = str;
			// Replace control codes with actual characters
			for (auto& code : controlCodes) {

				// replace all instances of the control code from the string
				size_t pos = 0;
				while ((pos = result.find(code, pos)) != std::string::npos) {
					std::string replacement = GetControlCodeReplacement(code);
					result.replace(pos, code.length(), replacement);
					pos += replacement.length();
				}
			}
			return result;
		}

		inline std::string GetControlCodeReplacement(const std::string code) {
			if (code == "<score>") {
				return std::to_string(Score::score * 1000);
			}
			else if (code == "<scoreboard>") {
				std::string scoreboard = "";
				Score::LoadHighscores();
				for (auto& entry : Score::highscores) {
					std::string initials = std::get<0>(entry).data();
					int score = std::get<1>(entry);
					if (score == 0) continue;
					initials = initials.substr(0, 3);
					scoreboard += initials + ": " + std::to_string(score * 1000) + "\n";
				}
				return scoreboard;
			}
			return code;
		}
	};
}