#include "LevelEditor.h"
#include "SceneManager.hpp"
#include "AudioManager.hpp"
#include "ObjectFactory.hpp"
#include "Time.hpp"
#include "Input.hpp"
#include "Game.h"
#include "Playfield.h"
#include "Player.h"
#include "Enemy.h"
#include "Snake.h"
#include "Ship.h"
#include "Stinger.h"
#include "Turret.h"
#include "Mimic.h"
#include "Scene.h"

void LevelEditor::Awake()
{
	currentScene = new Scene("CustomScene");
	allAvailableObjects = { new Enemy(), new Snake(), new Ship(), new Stinger(), new Turret(), new Mimic()};
}

void LevelEditor::Start()
{
	Game::GetInstance()->SetPaused(true);
}

void LevelEditor::Update()
{
	for (int i = 0; i < allAvailableObjects.size(); i++) {
		if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(i + 27))) {
			selectedObject = allAvailableObjects[i];
			break;
		}
	}

	if (Input::GetInstance()->GetKeyDown("Select")) {
		uiOpen = !uiOpen;
		if (uiOpen) OpenUi();
	}

	if (uiOpen) {
		DoUiStuff();
		return;
	}

	if (Input::GetInstance()->GetKeyDown("Start")) {
		playtesting = !playtesting;
		Game::GetInstance()->SetPaused(!playtesting);
		Scene* activeScene = SceneManager::GetInstance()->GetActiveScene();
		if (playtesting) {
			std::vector<Object*>* objects = currentScene->GetAllObjects();
			for (int i = 0; i < objects->size(); i++) {
				Object* obj = (*objects)[i];
				obj->tag = 1;
				activeScene->AddObject(ObjectFactory::CreateFromJson(obj->ToJson()));
			}
		}
		else {
			for (auto& obj : activeScene->GetObjectsWithTag(1)) {
				activeScene->RemoveObject(obj);
			}
		}
	}

	if (playtesting) {
		Playfield* playfield = Playfield::GetInstance();
		if (playfield->clearTimer > 0 && playfield->clearTimer < 2) {
			playfield->clearTimer = 0;
			playfield->wallArea.clear();
			playfield->mask->setRects(playfield->wallArea);
			Game::GetInstance()->SetPaused(!playtesting);
		}
	}

	// Get the mouse position relative to the window
	Vector2 mousepos = Input::GetInstance()->GetMousePos();
	sf::RenderWindow* window = Game::GetInstance()->GetWindow();
	sf::Vector2f worldpos = window->mapPixelToCoords(sf::Vector2i(mousepos.x, mousepos.y)) - (sf::Vector2f)window->getPosition();

	// place the object at the position on a 10x10 grid closest to the worldpos
	worldpos.x = std::round(worldpos.x / 10) * 10;
	worldpos.y = std::round(worldpos.y / 10) * 10;

	if (Input::GetInstance()->GetKeyDown("RightMouse")) {
		if (selectedObject != nullptr) {
			selectedObject = nullptr;
		}
		else {
			// Check if the mouse is over an object and delete it
			std::vector<Object*>* objects = currentScene->GetAllObjects();
			for (auto& obj : *objects) {
				if (obj->sprite.getGlobalBounds().contains(worldpos)) {
					obj->shouldDie = true;
					currentScene->Update();
					break;
				}
			}
		}
	}

	if (selectedObject != nullptr) {
		if (std::find(allAvailableObjects.begin(), allAvailableObjects.end(), selectedObject) != allAvailableObjects.end())
			selectedObject->position = Vector2(worldpos.x, worldpos.y);
		else return;
	}

	if (Input::GetInstance()->GetKeyDown("LeftMouse")) {
		if (selectedObject != nullptr) {
			Playfield* playfield = Playfield::GetInstance();
			if (!playfield->IsInBounds(selectedObject->position, false)) {
				std::cout << "Object is out of bounds!" << std::endl;
				return;
			}
			Object* newObject = ObjectFactory::CreateFromJson(selectedObject->ToJson());
			newObject->inactive = true;
			currentScene->AddObject(newObject);
			currentScene->Update();
			if (playtesting) {
				newObject->tag = 1;
				Scene* activeScene = SceneManager::GetInstance()->GetActiveScene();
				activeScene->AddObject(ObjectFactory::CreateFromJson(newObject->ToJson()));
			}
		}
		else {
			// Check if the mouse is over an object and select it
			std::vector<Object*>* objects = currentScene->GetAllObjects();
			for (auto& obj : *objects) {
				if (obj->sprite.getGlobalBounds().contains(worldpos)) {
					selectedObject = obj;
					break;
				}
			}
		}
	}
}

void LevelEditor::Draw(sf::RenderTarget& target)
{
	if (uiOpen) {
		// Draw the UI elements
		sf::RenderWindow* window = Game::GetInstance()->GetWindow();
		for (auto& ui : uiElements) {
			ui.second->draw(*window);
		}
		return;
	}
	if (selectedObject != nullptr) {
		selectedObject->Draw(target);
		// check if selectedObject is in allAvailableObjects, draw lines allong it's rectangle
		if (std::find(allAvailableObjects.begin(), allAvailableObjects.end(), selectedObject) == allAvailableObjects.end()) {
			sf::VertexArray line(sf::LineStrip, 5);
			sf::FloatRect rect = selectedObject->sprite.getGlobalBounds();
			line[0] = sf::Vertex(sf::Vector2f(rect.left, rect.top), sf::Color::Blue);
			line[1] = sf::Vertex(sf::Vector2f(rect.left + rect.width, rect.top), sf::Color::Blue);
			line[2] = sf::Vertex(sf::Vector2f(rect.left + rect.width, rect.top + rect.height), sf::Color::Blue);
			line[3] = sf::Vertex(sf::Vector2f(rect.left, rect.top + rect.height), sf::Color::Blue);
			line[4] = sf::Vertex(sf::Vector2f(rect.left, rect.top), sf::Color::Blue);
			target.draw(line);
		}
	}

	if (playtesting) return;

	currentScene->Draw(target);

	// Draw a 50x50 grid inside the playfield
	Playfield* playfield = Playfield::GetInstance();
	Vector2 extents = playfield->GetExtents();

	for (int x = -extents.x; x <= extents.x; x += 50) {
		for (int y = -extents.y; y <= extents.y; y += 50) {
			sf::Vertex horizontalLine[] = {
				sf::Vertex(sf::Vector2f(x + playfield->position.x, -extents.y + playfield->position.y), sf::Color::White),
				sf::Vertex(sf::Vector2f(x + playfield->position.x, extents.y + playfield->position.y), sf::Color::White)
			};
			sf::Vertex verticalLine[] = {
				sf::Vertex(sf::Vector2f(-extents.x + playfield->position.x, y + playfield->position.y), sf::Color::White),
				sf::Vertex(sf::Vector2f(extents.x + playfield->position.x, y + playfield->position.y), sf::Color::White)
			};
			target.draw(horizontalLine, 2, sf::Lines);
			target.draw(verticalLine, 2, sf::Lines);
		}
	}
}

void LevelEditor::SaveScene()
{
	std::filesystem::path cwd = std::filesystem::current_path();
	std::string path = cwd.string() + "/assets/scenes/custom/" + sceneName + ".json";
	nlohmann::json json = currentScene->ToJson();
	std::ofstream file(path);
	if (file.is_open()) {
		file << json.dump(4);
		file.close();
		std::cout << "Saved scene to: " << path << std::endl;
	}
	else {
		std::cerr << "Unable to open file: " << path << std::endl;
	}
}

void LevelEditor::LoadScene(std::string path)
{
	std::filesystem::path cwd = std::filesystem::current_path();
	std::ifstream file(cwd.string() + path);
	if (!file.is_open()) {
		std::cerr << "Could not open file: " << path << std::endl;
		return;
	}
	nlohmann::json json;
	file >> json;
	file.close();
	currentScene = Scene::CreateSceneFromJson(json);
	if (currentScene == nullptr) {
		std::cerr << "Could not load scene from: " << path << std::endl;
	}
}

void LevelEditor::DoUiStuff()
{
}

void LevelEditor::OpenUi()
{
    // Create default UI

	// scene name
	// save button
	// load button


	// Create Object variable UI

	// name
	// position
	// tag
	// subtags


	// Create Type specific UI

	// Enemy:
	// Ship:
	// Stinger:
	// [no extra parameters]

	// Snake:
	// length
	// segmentOffset

	// Turret:
	// direction
	// directionChange
	// shootTime

	// Mimic:
	// pathDrawInterval
}

