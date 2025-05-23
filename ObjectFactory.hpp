#pragma once
#include "json.hpp"
#include "Object.h"
#include "Playfield.h"
#include "Player.h"
#include "Enemy.h"
#include "Snake.h"
#include "Ship.h"
#include "LineFollower.h"
#include "CutsceneObject.hpp"
#include "Text.h"
#include "Selection.h"
#include "Cursor.h"
#include "Laser.h"
#include "Box.h"
#include "Stinger.h"
#include "AnimatedText.h"
#include "Turret.h"
#include "Mimic.h"
#include "LevelEditor.h"
#include "GunShip.h"
#include "Flail.h"

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
			Playfield* playfield = new Playfield();
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
		else if (type == "LineFollower") {
			LineFollower* lineFollower = new LineFollower();
			lineFollower->FromJson(json);
			return lineFollower;
		}
		else if (type == "CutsceneObject") {
			CutsceneObject* cutsceneObject = new CutsceneObject();
			cutsceneObject->FromJson(json);
			return cutsceneObject;
		}
		else if (type == "Text") {
			ui::Text* text = new ui::Text();
			text->FromJson(json);
			return text;
		}
		else if (type == "Selection") {
			ui::Selection* selection = new ui::Selection();
			selection->FromJson(json);
			return selection;
		}
		else if (type == "Cursor") {
			ui::Cursor* cursor = new ui::Cursor();
			cursor->FromJson(json);
			return cursor;
		}
		else if (type == "Laser") {
			Laser* laser = new Laser();
			laser->FromJson(json);
			return laser;
		}
		else if (type == "Box") {
			Box* box = new Box();
			box->FromJson(json);
			return box;
		}
		else if (type == "Stinger") {
			Stinger* stinger = new Stinger();
			stinger->FromJson(json);
			return stinger;
		}
		else if (type == "AnimatedText") {
			ui::AnimatedText* animatedText = new ui::AnimatedText();
			animatedText->FromJson(json);
			return animatedText;
		}
		else if (type == "Turret") {
			Turret* turret = new Turret();
			turret->FromJson(json);
			return turret;
		}
		else if (type == "Mimic") {
			Mimic* mimic = new Mimic();
			mimic->FromJson(json);
			return mimic;
		}
		else if (type == "LevelEditor") {
			LevelEditor* levelEditor = new LevelEditor();
			return levelEditor;
		}
		else if (type == "GunShip") {
			GunShip* gunShip = new GunShip();
			gunShip->FromJson(json);
			return gunShip;
		}
		else if (type == "Flail") {
			Flail* flail = new Flail();
			flail->FromJson(json);
			return flail;
		}

        return nullptr;
    }
};

