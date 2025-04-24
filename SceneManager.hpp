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

	inline void LoadScene(std::string name) {
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

		if (newScene != nullptr) {
			delete newScene;
		}
		newScene = Scene::CreateSceneFromJson(json);
		return;
	}

	inline Scene* GetActiveScene() { return activeScene; }

	inline void SwapSceneIfAvailable() {
		if (newScene == nullptr || newScene == activeScene) return;
		delete activeScene;
		activeScene = nullptr;
		activeScene = newScene;
		newScene = nullptr;
	}

private:
	SceneManager() { activeScene = nullptr; newScene = nullptr; };

	Scene* activeScene;
	Scene* newScene;
};