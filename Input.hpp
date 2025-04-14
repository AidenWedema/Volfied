#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <vector>
class Input
{
public:
	~Input() {};
	inline static Input* instance = nullptr;	
	inline static Input* GetInstance() {
		if (instance == nullptr)
			instance = new Input();
		return instance;
	};

	void Update() {
		for (auto& input : keyMap)
		{
			std::string button = input.first;
			for (auto& key : input.second)
			{
				if (sf::Keyboard::isKeyPressed(key))
				{
					inputs[button] = true;
					break;
				}
				else
					inputs[button] = false;
			}
		}
	};
	bool GetKey(std::string key) { return inputs[key]; };

private:
	Input() {
		keyMap["Up"] = { sf::Keyboard::W, sf::Keyboard::Up };
		keyMap["Down"] = { sf::Keyboard::S, sf::Keyboard::Down };
		keyMap["Left"] = { sf::Keyboard::A, sf::Keyboard::Left };
		keyMap["Right"] = { sf::Keyboard::D, sf::Keyboard::Right };
		keyMap["A"] = { sf::Keyboard::Space, sf::Keyboard::X };
		keyMap["B"] = { sf::Keyboard::LShift, sf::Keyboard::Z };
		keyMap["Start"] = { sf::Keyboard::Enter };
		keyMap["Select"] = { sf::Keyboard::LControl };
		keyMap["Speedup"] = { sf::Keyboard::Tab };
		keyMap["Reset"] = { sf::Keyboard::BackSpace };

		for (auto& input : keyMap)
			inputs[input.first] = false;
	};
	std::unordered_map<std::string, std::vector<sf::Keyboard::Key>> keyMap;
	std::unordered_map<std::string, bool> inputs;
};
