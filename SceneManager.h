#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "json.hpp"
#include "Scene.h"

class SceneManager
{
public:
	~SceneManager() {};
	static SceneManager* instance;
	static SceneManager* GetInstance();

	Scene* activeScene;

	void LoadScene(std::string name) {
		//load a json file with the scene data from assets/scenes
		std::filesystem::path cwd = std::filesystem::current_path();
		std::string path = cwd.string() + "/assets/scenes/" + name + ".json";
		std::ifstream file(path);
		if (!file.is_open()) {
			std::cerr << "Could not open file: " << path << std::endl;
			return;
		}

		nlohmann::json json;
		file >> json;
		file.close();

		activeScene = Scene::CreateSceneFromJson(json);
	}

private:
	SceneManager() { activeScene = nullptr; };
};