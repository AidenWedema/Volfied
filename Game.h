#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <chrono>
#include "Time.h"

class Game
{
public:
	~Game() {};

	static Game* instance;
	static Game* GetInstance();

	enum GameState { CLOSE, MENU, GAME, END, WIN };

	sf::RenderWindow window;

	void Start();
	void Run();

	void SetGameState(GameState state) { gameState = state; }
	GameState GetGameState() const { return gameState; }

private:
	Game() {};

	GameState gameState = MENU;
	bool running;
};