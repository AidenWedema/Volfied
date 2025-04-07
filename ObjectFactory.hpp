#pragma once
#include "json.hpp"
#include "Object.h"
#include "Playfield.h"
#include "Player.h"
#include "Enemy.h"
#include "Snake.h"
#include "Ship.h"

class ObjectFactory {
public:
    inline static Object* CreateFromJson(const nlohmann::json& json) {
        std::string type = json["type"];

        if (type == "Object") {
            Object* object = new Object();
            object->FromJson(json);
            return object;
		}
		else if (type == "Playfield") {
			Playfield* playfield = Playfield::GetInstance();
			playfield->FromJson(json);
			return playfield;
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
		else if (type == "Snake") {
			Snake* snake = new Snake();
			snake->FromJson(json);
			return snake;
		}
		else if (type == "Ship") {
			Ship* ship = new Ship();
			ship->FromJson(json);
			return ship;
		}
		
        return nullptr;
    }
};

