#include "TextInput.h"

void ui::TextInput::Awake()
{
	for (int i = 0; i < length; i++) {
		text.setString(text.getString() + 'a');
	}
	text.setCharacterSize(50);
	SetFont("assets/fonts/emulogic.ttf");
}

void ui::TextInput::Start()
{
	text.setFillColor(sf::Color(255, 255, 255, 255));
	SetPosition(position);
	//SetOrigin(Vector2(text.getGlobalBounds().width / 2, text.getGlobalBounds().height / 2));
}

void ui::TextInput::Update()
{
	if (Input::GetInstance()->GetKeyUp("Left")) {
		index--;
		if (index > length) index = 0;

		std::string s = text.getString();
		charindex = std::find(characters.begin(), characters.end(), s[index]) - characters.begin();
	}
	if (Input::GetInstance()->GetKeyUp("Right")) {
		index++;
		if (index >= length) index = length - 1;

		std::string s = text.getString();
		charindex = std::find(characters.begin(), characters.end(), s[index]) - characters.begin();
	}
	if (Input::GetInstance()->GetKeyUp("Up")) {
		charindex++;
		if (charindex >= characters.size()) charindex = 0;

		std::string s = text.getString();
		s[index] = characters[charindex];
		text.setString(s);
	}
	if (Input::GetInstance()->GetKeyUp("Down")) {
		charindex--;
		if (charindex > characters.size()) charindex = characters.size() - 1;

		std::string s = text.getString();
		s[index] = characters[charindex];
		text.setString(s);
	}
	if (Input::GetInstance()->GetKeyUp("Start")) {
		if (index >= length - 1) {
			char* s = new char[length + 1];
			std::string str = text.getString();
			for (int i = 0; i < length; i++) {
				s[i] = str[i];
			}

			Score::AddHighscore(s);
			Score::SaveHighscores();
			selection->Select();
			inactive = true;
		}
		else {
			index++;
			if (index >= length) index = length - 1;

			std::string s = text.getString();
			charindex = std::find(characters.begin(), characters.end(), s[index]) - characters.begin();
		}
	}
}

void ui::TextInput::Draw(sf::RenderTarget& target)
{
	if (clipped) return;

	std::string str = text.getString();
	float x = text.getPosition().x;
	float y = text.getPosition().y;
	float spacing = text.getLetterSpacing() * text.getCharacterSize() / 2.0f;

	sf::Text tempText = text; // Copy base style

	float width = (text.getCharacterSize() + text.getLetterSpacing()) * text.getString().getSize();
	float jump = width / text.getString().getSize();

	x -= width / 2;
	float offset = 0;
	for (int i = 0; i < length; i++)
	{
		offset = jump * i;
		tempText.setString(str.substr(i, 1));
		if (i == index)
			tempText.setFillColor(sf::Color::Yellow); // Highlight color
		else
			tempText.setFillColor(text.getFillColor());
		tempText.setPosition(x + offset, y);
		target.draw(tempText);
	}
}