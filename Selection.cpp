#include "Selection.h"
#include "Debug.hpp"
#include "SceneManager.hpp"

void ui::Selection::Select()
{
	for (auto& action : actions) {
		Debug::Log("Selected: " + std::to_string(action));
		switch (action)
		{
		case 0:
			SceneManager::GetInstance()->LoadScene("Empty Scene");
			break;
		default:
			break;
		}
	}
}
