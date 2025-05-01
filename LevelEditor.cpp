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

#include "TextElement.h"
#include "ButtonElement.h"
#include "InputElement.h"

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
	sf::RenderWindow* window = Game::GetInstance()->GetWindow();
	sf::Vector2f mousepos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
	float deltaTime = Time::GetInstance()->GetDeltaTime();
	for (auto& ui : uiElements) {
		ui.second->update(deltaTime);
	}
}

void LevelEditor::OpenUi()
{
    // Create default UI
    sf::Font font;
    if (!font.loadFromFile("assets/fonts/Arial.ttf")) {
        return;
    }

    // Title
    TextElement* titletext = new TextElement(sf::Vector2f(20, 20), "Options", font);
    uiElements["title"] = titletext;

    // Save / Load Buttons
    ButtonElement* saveButton = new ButtonElement(sf::Vector2f(20, 70), sf::Vector2f(100, 40), "Save", font);
    saveButton->addCallback([this]() {
        SaveScene();
    });
    ButtonElement* loadButton = new ButtonElement(sf::Vector2f(130, 70), sf::Vector2f(100, 40), "Load", font);
    loadButton->addCallback([this]() {
        LoadScene("/assets/scenes/custom/" + sceneName + ".json");
    });
    uiElements["save"] = saveButton;
    uiElements["load"] = loadButton;

    // Scene name input
    TextElement* sceneNameLabel = new TextElement(sf::Vector2f(20, 120), "Scene Name:", font);
    InputElement* sceneNameInput = new InputElement(sf::Vector2f(130, 120), sceneName, font);
	ButtonElement* sceneNameButton = new ButtonElement(sf::Vector2f(240, 120), sf::Vector2f(100, 40), "Set", font);
	sceneNameButton->addCallback([this, sceneNameInput]() {
		sceneName = sceneNameInput->getText();
		});
    uiElements["sceneNameLabel"] = sceneNameLabel;
    uiElements["sceneNameInput"] = sceneNameInput;

    // Object variables
	if (selectedObject == nullptr) return;
	std::string objType = std::string(typeid(*selectedObject).name());

	TextElement* objectLabel = new TextElement(sf::Vector2f(20, 170), "Object Variables:", font);
	uiElements["objectLabel"] = objectLabel;

	// Object name input
	InputElement* objectNameInput = new InputElement(sf::Vector2f(20, 200), selectedObject->name, font);
	ButtonElement* objectNameButton = new ButtonElement(sf::Vector2f(240, 200), sf::Vector2f(100, 40), "Set name", font);
	objectNameButton->addCallback([this, objectNameInput]() {
		selectedObject->name = objectNameInput->getText();
		});
	uiElements["objectNameInput"] = objectNameInput;
	uiElements["objectNameButton"] = objectNameButton;

	// Object position input
	InputElement* objectPosXInput = new InputElement(sf::Vector2f(20, 250), std::to_string(selectedObject->position.x), font);
	InputElement* objectPosYInput = new InputElement(sf::Vector2f(130, 250), std::to_string(selectedObject->position.y), font);
	ButtonElement* objectPosButton = new ButtonElement(sf::Vector2f(240, 250), sf::Vector2f(100, 40), "Set position", font);
	objectPosButton->addCallback([this, objectPosXInput, objectPosYInput]() {
		selectedObject->position.x = std::stof(objectPosXInput->getText());
		selectedObject->position.y = std::stof(objectPosYInput->getText());
		});
	uiElements["objectPosXInput"] = objectPosXInput;
	uiElements["objectPosYInput"] = objectPosYInput;
	uiElements["objectPosButton"] = objectPosButton;

	// Object tag input
	InputElement* objectTagInput = new InputElement(sf::Vector2f(20, 300), std::to_string(selectedObject->tag), font);
	ButtonElement* objectTagButton = new ButtonElement(sf::Vector2f(240, 300), sf::Vector2f(100, 40), "Set tag", font);
	objectTagButton->addCallback([this, objectTagInput]() {
		selectedObject->tag = std::stoi(objectTagInput->getText());
		});
	uiElements["objectTagInput"] = objectTagInput;
	uiElements["objectTagButton"] = objectTagButton;

	// Object subtags input
    InputElement* objectSubtagInput = new InputElement(sf::Vector2f(20, 350), "", font);
    ButtonElement* objectSubtagButton = new ButtonElement(sf::Vector2f(240, 350), sf::Vector2f(100, 40), "Set subtags", font);
    objectSubtagButton->addCallback([this, objectSubtagInput]() {
       std::string input = objectSubtagInput->getText();
	   std::vector<int> subtags;


       selectedObject->subTags = subtags;
    });
    uiElements["objectSubtagInput"] = objectSubtagInput;
    uiElements["objectSubtagButton"] = objectSubtagButton;
}

