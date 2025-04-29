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

	inline void Update() {
		scroll = 0.0f;

		Vector2 newMousePos = Vector2(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y);
		mouseDrag = newMousePos - mousePos;
		mousePos = newMousePos;

		prevInputs = inputs;
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
		for (auto& input : mouseMap)
		{
			std::string button = input.first;
			for (auto& key : input.second)
			{
				if (sf::Mouse::isButtonPressed(key))
				{
					inputs[button] = true;
					break;
				}
				else
					inputs[button] = false;
			}
		}
		pressedKeys.clear();
		releasedKeys.clear();
		for (auto& input : inputs)
		{
			if (input.second && !prevInputs[input.first])
			{
				pressedKeys.push_back(input.first);
			}
			else if (!input.second && prevInputs[input.first])
			{
				releasedKeys.push_back(input.first);
			}
		}
	};

	/// <summary>
	/// Check if any input is pressed
	/// </summary>
	inline bool Any() {
		if (scroll != 0.0f) return true;
		for (auto& input : inputs)
		{
			if (input.second)
				return true;
		}
		return false;
	}

	/// <summary>
	/// Returns true if the given input is pressed
	/// </summary>
	inline bool GetKey(std::string key) { return inputs[key]; };

	/// <summary>
	/// Returns true on the first frame the key is pressed
	/// </summary>
	inline bool GetKeyDown(std::string key) {
		if (pressedKeys.size() == 0) return false;
		for (auto& input : pressedKeys)
		{
			if (input == key)
				return true;
		}
		return false;
	};

	/// <summary>
	/// Returns true on the first frame the key is released
	/// </summary>
	inline bool GetKeyUp(std::string key) {
		if (releasedKeys.size() == 0) return false;
		for (auto& input : releasedKeys)
		{
			if (input == key)
				return true;
		}
		return false;
	};

	/// <summary>
	/// Returns the scroll amount of the mouse wheel
	/// </summary>
	inline float GetScroll() const { return scroll; };

	/// <summary>
	/// Returns the mouse position in screen coordinates
	/// </summary>
	inline Vector2 GetMousePos() const { return mousePos; };

	/// <summary>
	/// Returns the amount the mouse has moved since the last frame
	/// </summary>
	inline Vector2 GetMouseDrag() const { return mouseDrag; };

private:
	Input() {
		scroll = 0.0f;
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

		mouseMap["LeftMouse"] = { sf::Mouse::Left };
		mouseMap["RightMouse"] = { sf::Mouse::Right };
		mouseMap["MiddleMouse"] = { sf::Mouse::Middle };

		for (auto& input : keyMap)
			inputs[input.first] = false;
		for (auto& input : mouseMap)
			inputs[input.first] = false;
	};
	std::unordered_map<std::string, std::vector<sf::Keyboard::Key>> keyMap;
	std::unordered_map<std::string, std::vector<sf::Mouse::Button>> mouseMap;
	std::unordered_map<std::string, bool> inputs;
	std::unordered_map<std::string, bool> prevInputs;
	std::vector<std::string> pressedKeys;
	std::vector<std::string> releasedKeys;
	float scroll;
	Vector2 mousePos;
	Vector2 mouseDrag;
};