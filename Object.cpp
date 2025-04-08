#include "Object.h"
#include "ObjectFactory.hpp"
#include "SceneManager.hpp"

void Object::Destroy(Object* obj) {
	if (obj == nullptr) return;
	obj->shouldDie = true;
}

Object* Object::Instantiate(std::string path)
{
	std::filesystem::path cwd = std::filesystem::current_path();
	path = cwd.string() + "/assets/" + path + ".json";
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "Could not open file: " << path << std::endl;
		return nullptr;
	}

	nlohmann::json json;
	file >> json;
	file.close();

	Object* object = ObjectFactory::CreateFromJson(json);
	SceneManager::GetInstance()->GetActiveScene()->AddObject(object);
	return object;
}