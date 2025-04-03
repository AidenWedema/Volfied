#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "Object.h"
#include "ObjectFactory.h"

class Scene
{
public:
	Scene() : name("Scene") {};
	Scene(std::string name) : name(name){};
	~Scene() {};

	std::string name;

    static Scene* CreateSceneFromJson(const nlohmann::json& json) {
		Scene* scene = new Scene();
		scene->FromJson(json);
		return scene;
    }

    void SaveToJson() {
        std::filesystem::path cwd = std::filesystem::current_path();
        std::string path = cwd.string() + "/assets/scenes/" + name + ".json";

        std::ofstream file(path);
        if (file.is_open()) {
            file << ToJson().dump(4);
            file.close();
            std::cout << "Saved scene to: " << path << std::endl;
        }
        else {
            std::cerr << "Unable to open file: " << path << std::endl;
        }
    }

	void Update() {
        for (Object* obj : newObjects) {
            obj->Start();
            objects.push_back(obj);
        }
		newObjects.clear();

		for (Object* obj : objects)
			obj->Update();
	}

    void Draw(sf::RenderTarget& target) {
        for (Object* obj : objects)
            obj->Draw(target);
    }

    nlohmann::json ToJson() {
        nlohmann::json json;
        json["type"] = "Scene";
		json["name"] = name;
        json["objects"] = nlohmann::json::array();

		for (const auto& obj : newObjects) {
			json["objects"].push_back(obj->ToJson());
		}
        for (const auto& obj : objects) {
            json["objects"].push_back(obj->ToJson());
        }
        return json;
    }

    void FromJson(const nlohmann::json& json) {
        name = json["name"];
        for (const auto& objJson : json["objects"]) {
            Object* obj = ObjectFactory::CreateFromJson(objJson);
            if (obj) {
                AddObject(obj);
            }
        }
    }

    void AddObject(Object* object) {
        newObjects.push_back(object);
    }

private:
    std::vector<Object*> objects;
    std::vector<Object*> newObjects;
};

