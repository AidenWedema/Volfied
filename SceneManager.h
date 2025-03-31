#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "json.hpp"
#include "Scene.h"

class SceneManager
{
public:
	static Scene* activeScene;

	static void LoadScene(std::string name) {
		//load a json file with the scene data from assets/scenes
		std::string path = "assets/scenes/" + name + ".json";
		std::ifstream file(path);
		nlohmann::json json = nlohmann::json::parse(file);
		activeScene = Scene::CreateSceneFromJson(json);
	}
};