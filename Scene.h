#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "Object.h"
#include "ObjectFactory.hpp"

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
		if (newObjects.size() > 0) {
			newObjects.clear();
			// Sort objects by their tag
			std::sort(objects.begin(), objects.end(), [](Object* a, Object* b) { return a->tag < b->tag; });
		}

		// Remove objects that should die
		for (auto it = objects.begin(); it != objects.end();) {
			if ((*it)->shouldDie) {
				delete* it;
				it = objects.erase(it);
			}
			else {
				++it;
			}
		}

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

	void RemoveObject(Object* object) {
		auto it = std::find(objects.begin(), objects.end(), object);
		if (it != objects.end()) {
			objects.erase(it);
			delete object;
		}
	}

	Object* GetObjectByName(std::string name) {
		for (Object* obj : objects) {
			if (obj->name == name) {
				return obj;
			}
		}
	}

	std::vector<Object*> GetObjectsByName(std::string name) {
		std::vector<Object*> foundObjects;
		for (Object* obj : objects) {
			if (obj->name == name) {
				foundObjects.push_back(obj);
			}
		}
		return foundObjects;
	}

    std::vector<Object*> GetObjectsWithTag(int tag) {
        std::vector<Object*> foundObjects;
		for (Object* obj : objects) {
			if (obj->tag == tag) {
				foundObjects.push_back(obj);
			}
		}
        return foundObjects;
    }

private:
    std::vector<Object*> objects;
    std::vector<Object*> newObjects;
};

