#pragma once
#include "Object.h"

class BabyShip : public Object
{
friend class MotherShip;
public:
   BabyShip() : Object("BabyShip") { Awake(); };
   BabyShip(Vector2 position) : Object("BabyShip", position) { Awake(); };
   ~BabyShip() {};

   void Awake() override;
   void Start() override;
   void Update() override;
   void Draw(sf::RenderTarget& target) override;
   void OnDestroy() override;

   nlohmann::json ToJson() const override {
       return {
           {"type", "BabyShip"},  
           {"name", name},  
           {"tag", tag},  
           {"subTags", subTags},  
           {"position", {position.x, position.y}}
       };
   }
   void FromJson(const nlohmann::json& json) override {
       name = json["name"];
       tag = json["tag"];
       subTags = json["subTags"].get<std::vector<int>>();
       position = Vector2(json["position"][0], json["position"][1]);
   }

private:
   MotherShip* motherShip = nullptr;
   float speed;
   float maxSpeed = 6;
   float acceleration = 0.05f;
   int angle = 0;
   float rotationSpeed = 5;
   bool searching = false;
   bool returning = false;
};

class MotherShip : public Object
{
public:
   MotherShip() : Object("MotherShip") { Awake(); };
   MotherShip(Vector2 position) : Object("MotherShip", position) { Awake(); };
   ~MotherShip() {};

   void Awake() override;
   void Start() override;
   void Update() override;
   void OnDestroy() override;

   nlohmann::json ToJson() const override {
       return {
           {"type", "MotherShip"},  
           {"name", name},  
           {"tag", tag},  
           {"subTags", subTags},  
           {"position", {position.x, position.y}}
       };
   }
   void FromJson(const nlohmann::json& json) override {
       name = json["name"];
       tag = json["tag"];
       subTags = json["subTags"].get<std::vector<int>>();
       position = Vector2(json["position"][0], json["position"][1]);
   }

   inline void RemoveBaby(BabyShip* baby) {
	   auto it = std::find(babies.begin(), babies.end(), baby);
	   if (it != babies.end()) *it = nullptr;
   }

private:
   std::array<BabyShip*, 8> babies;
   int speed = 1;
   int angle;
   float timer = 0.0f;
   int mode;
};
