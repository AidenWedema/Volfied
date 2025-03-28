#pragma once
#include <string>
#include <vector>
#include "Object.h"

class Scene
{
public:
	std::string name;
	std::vector<Object*> objects;
};

