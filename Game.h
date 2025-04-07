#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <chrono>
#include "Time.hpp"
#include "SceneManager.hpp"
#include "Input.hpp"
#include "Player.h"
#include "RNG.hpp"

class Game
{
public:
	~Game() {};

	inline static Game* instance = nullptr;
	inline static Game* GetInstance()
	{
		if (instance == nullptr)
			instance = new Game();
		return instance;
	}

	enum GameState { CLOSE, MENU, GAME, END, WIN };

	void Start();
	void Run();

	void SetGameState(GameState state) { gameState = state; }
	GameState GetGameState() const { return gameState; }

	sf::RenderWindow* GetWindow() { return &window; }

private:
	Game() : running(true) {};

	sf::RenderWindow window;
	GameState gameState = MENU;
	bool running;
};