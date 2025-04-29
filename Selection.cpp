#include "Selection.h"
#include "Debug.hpp"
#include "SceneManager.hpp"
#include "Game.h"
#include <thread>
#include <chrono>

void ui::Selection::Select()
{
	for (auto& action : actions) {
		switch (action)
		{
			// Load next level
		case 0:
			Game::GetInstance()->SetLevel(Game::GetInstance()->GetLevel() + 1);
			SceneManager::GetInstance()->LoadScene("Level-" + std::to_string(Game::GetInstance()->GetLevel()));
			break;

			// Quit game
		case 1:
			Game::GetInstance()->SetGameState(Game::CLOSE);
			break;

			// Restart game
		case 2:
			Game::GetInstance()->SetGameState(Game::END);
			break;

			// Load next cutscene
		case 3:
			SceneManager::GetInstance()->LoadScene("Cutscene-" + std::to_string(Game::GetInstance()->GetLevel()));
			break;

			// Load main menu
		default:
			break;
		}
	}
}
