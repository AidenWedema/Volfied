#include "Selection.h"
#include "Debug.hpp"
#include "SceneManager.hpp"
#include "Game.h"

void ui::Selection::Select()
{
	for (auto& action : actions) {
		switch (action)
		{
		case 0:
			Game::GetInstance()->SetLevel(Game::GetInstance()->GetLevel() + 1);
			SceneManager::GetInstance()->LoadScene("Level-" + std::to_string(Game::GetInstance()->GetLevel()));
			break;
		default:
			break;
		}
	}
}
