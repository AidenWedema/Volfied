#pragma once
#include "Text.h"
#include "Time.hpp"

namespace ui
{
	class AnimatedText : public Text
	{
	public:
		AnimatedText() : Text() {};
		~AnimatedText() {};

		std::string fullText = "";	// The entire string to be displayed
		int textLength = 0;			// The current length of the displayed string
		float timer = 0;			// Timer for the animation
		float delay = 0.1f;			// Delay in seconds between each new character
		int textWidth = 0;			// The width of the text before wrapping

		inline nlohmann::json ToJson() {
			return {
				{"type", "Text"},
				{"name", name},
				{"tag", tag},
				{"subTags", subTags},
				{"position", {position.x, position.y}},
				{"text", fullText},
				{"font", font.getInfo().family},
				{"size", text.getCharacterSize()},
				{"color", {text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, text.getFillColor().a}},
				{"delay", delay},
				{"textWidth", textWidth}
			};
		}

		inline void FromJson(const nlohmann::json& json) {
			name = json["name"];
			tag = json["tag"];
			subTags = json["subTags"].get<std::vector<int>>();
			SetPosition(Vector2(json["position"][0], json["position"][1]));
			if (json.contains("text")) fullText = static_cast<std::string>(json["text"]);
			if (json.contains("font")) {
				font.loadFromFile(json["font"]);
				text.setFont(font);
			}
			if (json.contains("size")) text.setCharacterSize(json["size"]);
			if (json.contains("color")) text.setFillColor(sf::Color(json["color"][0], json["color"][1], json["color"][2], json["color"][3]));
			if (json.contains("delay")) delay = json["delay"];
			if (json.contains("textWidth")) textWidth = json["textWidth"];
		}

		void Awake() override
		{
			if (!font.loadFromFile("assets/fonts/arial.ttf"))
			{
				std::cerr << "Error loading font\n";
				return;
			}
			text.setFont(font);
			text.setCharacterSize(24);
			text.setFillColor(sf::Color::White);
			text.setPosition(position.x, position.y);
			timer = delay;
		}

		void Start() override
		{
			sprite.setPosition(position.x, position.y);
		}

		void Update() override
		{
			if (inactive) return;

			timer -= Time::GetInstance()->GetDeltaTime();
			if (timer <= 0)
			{
				timer = delay;
				if (textLength < fullText.length())
				{
					textLength++;
					text.setString(fullText.substr(0, textLength));
					// wrap text by word
					if (text.getGlobalBounds().width > textWidth) {
						size_t lastSpace = text.getString().toAnsiString().rfind(' ', textLength - 1);
						fullText.replace(lastSpace, 1, "\n");
						textLength++;
						text.setString(fullText.substr(0, textLength));
					}
				}
			}
		}
	};
}
