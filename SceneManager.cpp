#include "SceneManager.h"
SceneManager* SceneManager::instance = nullptr;
SceneManager* SceneManager::GetInstance()
{
	if (instance == nullptr)
		instance = new SceneManager();
	return instance;
}