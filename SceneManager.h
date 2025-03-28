#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "Scene.h"

class SceneManager
{
public:
	static Scene* activeScene;

	static void LoadScene(std::string name) {

	}
};