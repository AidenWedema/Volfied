#pragma once
#include <string>
#include <vector>
#include "json.hpp"
#include "Object.h"

class Scene
{
public:
	std::string name;
	std::vector<Object*> objects;

	static Scene* CreateSceneFromJson(nlohmann::json json) {
		Scene* scene = new Scene();
		scene->name = json["name"];
		// Load all the objects in the scene
		for (nlohmann::json::iterator it = json["objects"].begin(); it != json["objects"].end(); ++it) {
			Object* object = new Object();
			object->name = it.value()["name"];
			object->position = Vector2(it.value()["position"][0], it.value()["position"][1]);
			scene->objects.push_back(object);
		}
		return scene;
	}

};

