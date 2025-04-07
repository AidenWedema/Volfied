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
	inline static SceneManager* instance = nullptr;
	inline static SceneManager* GetInstance() {
		if (instance == nullptr)
			instance = new SceneManager();
		return instance;
	};

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

	Scene* GetActiveScene() {
		if (activeScene == nullptr)
			activeScene = new Scene();
		return activeScene;
	}

private:
	SceneManager() { activeScene = nullptr; };

	Scene* activeScene;
};