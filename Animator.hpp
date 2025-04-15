#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "json.hpp"
#include "Vector2.hpp"
#include "Time.hpp"


struct Animation {
	Animation() {};
	Animation(std::string path) : frameTime(0), frame(0), loopmode(1), useDeltatime(false) {		
		std::filesystem::path cwd = std::filesystem::current_path();
		std::string p = cwd.string() + "/" + path + ".json";
		std::ifstream file(p);
		if (!file.is_open()) {
			std::cerr << "Could not open file: " << p << std::endl;
			return;
		}

		nlohmann::json json;
		file >> json;
		file.close();
		FromJson(json);

		time = frameTime;
		if (!sheet.loadFromFile(path)) {
			std::cerr << "Error loading texture from path: " << path << std::endl;
			return;
		}
		sprite.setTexture(sheet);
		sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(frameSize.x), static_cast<int>(frameSize.y)));
		sprite.setOrigin(frameSize.x / 2, frameSize.y / 2);
	}

	sf::Texture sheet;  // The texture sheet for the animation
	sf::Sprite sprite;	// The sprite to display
	Vector2 frameSize;  // The size of each frame in the texture sheet
	float frameTime;	// The time it takes to play one frame of the animation
	float time;			// The current time in the animation
	int frame;			// The current frame of the animation
	int loopmode;		// The loop mode of the animation, 0 = no loop, 1 = loop, 2 = ping pong, 3 = ping pong reverse
	bool useDeltatime;	// Whether to use delta time for animation. if false, playtime and time are in counted in frames, if true, playtime and time are counted in seconds

	inline void FromJson(nlohmann::json& json) {
		frameSize = Vector2(json["frameSize"][0], json["frameSize"][1]);
		frameTime = json["frameTime"];
		loopmode = json["loopmode"];
		useDeltatime = json["useDeltatime"];
	}

	inline void Update() {
		if (useDeltatime) time -= Time::GetInstance()->GetDeltaTime();
		else time -= 1;

		if (time <= 0) {
			time = frameTime;
			frame++;
			if (frame * frameSize.y >= sheet.getSize().y) {
				if (loopmode == 0) {
					frame--;
				}
				else if (loopmode == 1) {
					frame = 0;
				}
				else if (loopmode == 2) {
					frame = sheet.getSize().y / frameSize.y - 1;
					loopmode = 3;
				}
				else if (loopmode == 3) {
					frame = 0;
					loopmode = 2;
				}
			}
			sprite.setTexture(sheet);
			sprite.setTextureRect(sf::IntRect(0, static_cast<int>(frame * frameSize.y), static_cast<int>(frameSize.x), static_cast<int>(frameSize.y)));
		}
	}
};

class Animator
{
public:
	Animator(bool autoUpdate=true) : current(nullptr), autoUpdate(autoUpdate) {};
	~Animator() {};

	Animation* current;									   // The animation that's currently playing
	sf::Sprite* sprite;									   // The sprite to display the animation on
	std::unordered_map<std::string, Animation> animations; // A map of all animations by name
	Vector2 position;									   // The position of the animator
	bool autoUpdate;									   // Whether to call Animation::Update() every time Draw() is called

	inline void AddAnimation(std::string name, std::string path) {
		if (animations.find(name) != animations.end())
			return;
		animations[name] = Animation(path);
	}

	inline void SetAnimation(std::string name) {
		if (animations.find(name) == animations.end())
			return;
		current = &animations[name];
		current->frame = 0;
		current->time = current->frameTime;
		sprite = &current->sprite;
	}

	inline void RemoveAnimation(std::string name) {
		if (animations.find(name) == animations.end())
			return;
		if (current == &animations[name]) {
			current = nullptr;
			sprite = nullptr;
		}
		animations.erase(name);
	}

	inline void Draw(sf::RenderTarget& target) {
		if (autoUpdate) current->Update();
		if (current != nullptr) {
			sprite = &current->sprite;
			sprite->setPosition(position.x, position.y);
			target.draw(*sprite);
		}
	}
};

