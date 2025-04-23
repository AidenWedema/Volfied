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

	inline void SetGameState(GameState state) { gameState = state; }
	inline GameState GetGameState() const { return gameState; }

	inline sf::RenderWindow* GetWindow() { return &window; }

	inline void SetPaused(bool pause) { 
		for (auto& obj : *SceneManager::GetInstance()->GetActiveScene()->GetAllObjects()) {
			obj->inactive = pause;
		}
	}

private:
	Game() : running(true) {};

	sf::RenderWindow window;
	GameState gameState = MENU;
	bool running;
};