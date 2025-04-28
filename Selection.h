#pragma once  
#include <string>  
#include <vector>  
#include <SFML/Graphics.hpp>  
#include "Text.h"

namespace ui {  
   class Selection : public Text  
   {
   public:
       Selection() {};  
       ~Selection() {};  

       std::vector<int> actions;

       inline void Draw(sf::RenderTarget& target) override
       {
           if (clipped) return;
           target.draw(text);
       }

       inline nlohmann::json ToJson() {
           return {
               {"type", "Text"},
               {"name", name},
               {"tag", tag},
               {"subTags", subTags},
               {"position", {position.x, position.y}},
               {"text", text.getString()},
               {"font", font.getInfo().family},
               {"size", text.getCharacterSize()},
			   {"color", {text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, text.getFillColor().a}},
			   {"actions", actions}
           };
       }  

       inline void FromJson(const nlohmann::json& json) {  
           name = json["name"];  
           tag = json["tag"];  
           subTags = json["subTags"].get<std::vector<int>>();  
           SetPosition(Vector2(json["position"][0], json["position"][1]));
		   if (json.contains("actions")) actions = json["actions"].get<std::vector<int>>();
           if (json.contains("text")) text.setString(static_cast<std::string>(json["text"]));  
           if (json.contains("font")) {  
               font.loadFromFile(json["font"]);  
               text.setFont(font);  
           }  
           if (json.contains("size")) text.setCharacterSize(json["size"]);  
           if (json.contains("color")) text.setFillColor(sf::Color(json["color"][0], json["color"][1], json["color"][2], json["color"][3]));  
       }  

       void Select();
   };
}