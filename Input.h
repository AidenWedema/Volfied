#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <vector>
class Input
{
public:
	~Input() {};
	static Input* instance;	
	static Input* GetInstance();

	void Update();
	bool GetKey(std::string key);

private:
	Input();
	std::unordered_map<std::string, std::vector<sf::Keyboard::Key>> keyMap;
	std::unordered_map<std::string, bool> inputs;
};
