#pragma once
#include "json.hpp"
#include "Object.h"
#include "Player.h"
#include "Enemy.h"
#include "Playfield.h"

class ObjectFactory {
public:
    static Object* CreateFromJson(const nlohmann::json& json) {
        std::string type = json["type"];

        if (type == "Object") {
            Object* object = new Object();
            object->FromJson(json);
            return object;
		}
		else if (type == "Player") {
			Player* player = new Player();
			player->FromJson(json);
			return player;
		}
		else if (type == "Enemy") {
			Enemy* enemy = new Enemy();
			enemy->FromJson(json);
			return enemy;
		}
		else if (type == "Playfield") {
			Playfield* playfield = Playfield::GetInstance();
			playfield->FromJson(json);
			return playfield;
		}

        return nullptr;
    }
};

