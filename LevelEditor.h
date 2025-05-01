#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include "json.hpp"
#include "Object.h"

class Scene;

class LevelEditor : public Object
{
public:
	LevelEditor() : Object("LevelEditor") { Awake(); };
	~LevelEditor() {
		for (auto& ui : uiElements) {
			delete ui.second;
		}
		uiElements.clear();
		if (currentScene != nullptr) {
			delete currentScene;
			currentScene = nullptr;
		}
		for (auto& obj : allAvailableObjects) {
			delete obj;
		}
		allAvailableObjects.clear();
		selectedObject = nullptr;
	};

	inline static LevelEditor* instance = nullptr;
	inline static LevelEditor* GetInstance() {
		if (instance == nullptr)
			instance = new LevelEditor();
		return instance;
	};

	void Awake() override;
	void Start() override;
	void Update() override;
	void Draw(sf::RenderTarget& target) override;
	void SaveScene();
	void LoadScene(std::string path);
	void DoUiStuff();
	void OpenUi();

private:
	std::string sceneName;
	Object* selectedObject = nullptr;
	std::vector<Object*> allAvailableObjects;
	Scene* currentScene = nullptr;
	bool uiOpen = false;
	bool playtesting = false;
};