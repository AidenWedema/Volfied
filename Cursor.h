#pragma once
#include "Object.h"
#include "Selection.h"
#include "Input.hpp"

namespace ui {
	class Cursor : public Object
	{
	public:
		Cursor() {};
		~Cursor() {};

		struct Index
		{
			ui::Selection* select;
			Vector2 position;
			std::unordered_map<Vector2, Index*> connections;
		};

		inline void Start()
		{
			if (!texture.loadFromFile("assets/sprites/player.png")) {
				std::cerr << "Error loading player texture\n";
				return;
			}
			sprite.setTexture(texture);
			sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
			selected = &options[0];
		}

		inline void Update() override
		{
			if (inactive) return;

			if (Input::GetInstance()->GetKey("Left")) Move(Vector2::Left());
			if (Input::GetInstance()->GetKey("Right")) Move(Vector2::Right());
			if (Input::GetInstance()->GetKey("Up")) Move(Vector2::Up());
			if (Input::GetInstance()->GetKey("Down")) Move(Vector2::Down());
			if (Input::GetInstance()->GetKey("Start")) {
				if (selected != nullptr || selected->select != nullptr)
					selected->select->Select();
			}
		}

		inline void Draw(sf::RenderTarget& target) override
		{
			if (clipped) return;

			for (int i = 0; i < options.size(); i++) {
				options[i].select->Draw(target);
			}

			position = selected->position;
			sprite.setPosition(position.x, position.y);
			target.draw(sprite);
		}

		std::vector<Index> options;
		Index* selected;

		inline nlohmann::json ToJson() {
			return {
				{"type", "Cursor"},
				{"name", name},
				{"tag", tag},
				{"subTags", subTags},
				{"position", {position.x, position.y}},
			};
		}

		inline void FromJson(const nlohmann::json& json) {
			name = json["name"];
			tag = json["tag"];
			subTags = json["subTags"].get<std::vector<int>>();
			position = Vector2(json["position"][0], json["position"][1]);
			for (auto& option : json["options"]) {
				Index index;
				index.select = new ui::Selection();
				index.select->FromJson(option);
				index.select->SetOrigin(Vector2(index.select->text.getLocalBounds().width / 2, index.select->text.getLocalBounds().height / 2));
				index.position = Vector2(option["cursorPosition"][0], option["cursorPosition"][1]);
				options.push_back(index);
			}
			for (int i = 1; i < options.size(); i++) {
				options[i - 1].connections[Vector2::Down()] = &options[i];
				options[i].connections[Vector2::Up()] = &options[i - 1];
			}
		}

        void Move(Vector2 direction)
        {
			if (selected == nullptr || selected->connections.find(direction) == selected->connections.end())
				return;

			selected = selected->connections[direction];
        }
	};
}

