#pragma once
#include <vector>
#include "Text.h"
#include "Input.hpp"
#include "Selection.h"

namespace ui
{
	class TextInput : public Text
	{
	public:
		TextInput() : Text() { Awake(); };
		~TextInput() {};

		void Awake() override;
		void Start() override;
		void Update() override;
		void Draw(sf::RenderTarget& target) override;

		nlohmann::json ToJson() const override {
			return {
				{"type", "TextInput"},
				{"name", name},
				{"tag", tag},
				{"subTags", subTags},
				{"position", {position.x, position.y}},
				{"text", text.getString()},
				{"font", font.getInfo().family},
				{"size", text.getCharacterSize()},
				{"color", {text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, text.getFillColor().a}},
				{"option", selection->ToJson()},
				{"length", length},
			};
		}

		void FromJson(const nlohmann::json& json) override {
			name = json["name"];
			position = Vector2(json["position"][0], json["position"][1]);
			selection = new ui::Selection();
			selection->FromJson(json["option"]);
			length = json["length"];
		}

	private:
		std::vector<char> characters = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
			'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '!', '?', '*', ' '};
		uint8_t index;
		uint8_t length = 3;
		uint8_t charindex = 0;
		ui::Selection* selection = nullptr;
	};
}

