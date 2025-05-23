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

			// Toggle fullscreen
		case 4: {
			sf::RenderWindow* window = Game::GetInstance()->GetWindow();
			sf::View defaultView = window->getDefaultView();
			sf::View currentView = window->getView();
			if (defaultView.getCenter() == currentView.getCenter() || defaultView.getSize() == currentView.getSize()) {
				sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
				window->create(sf::VideoMode(desktop.width, desktop.height), "Volfied", sf::Style::Fullscreen);
				sf::View view(sf::Vector2f(400, 300), sf::Vector2f(800, 600));
				window->setView(view);
			}
			else
			{
				window->create(sf::VideoMode(800, 600), "Volfied", sf::Style::Close);
			}
			break;
		}

			// Load main menu
		default:
			break;
		}
	}
}
