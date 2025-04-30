#pragma once
#include <SFML/Audio.hpp>
#include <vector>

class AudioManager
{
public:
	inline static void PlayMusic(std::string path)
	{
		if (!music->openFromFile(path)) {
			return;
		}
		music->setLoop(true);
		music->play();
	}

	inline static void StopMusic()
	{
		music->stop();
	}

	inline static void PlaySound(std::string path)
	{
		// Remove sounds that finished playing
		for (auto it = sounds.begin(); it != sounds.end();) {
			if ((*it)->getStatus() == sf::Sound::Stopped) {
				delete* it;
				it = sounds.erase(it);
			}
			else {
				++it;
			}
		}

		sf::Sound* sound = new sf::Sound();
		buffer = new sf::SoundBuffer();
		if (!buffer->loadFromFile(path)) {
			return;
		}
		sound->setBuffer(*buffer);
		sound->play();
		sounds.push_back(sound);
	}

	inline static void StopSound()
	{
		for (auto& sound : sounds) {
			sound->stop();
		}

		for (auto it = sounds.begin(); it != sounds.end();) {
			delete* it;
			it = sounds.erase(it);
		}
	}

private:
	inline static sf::Music* music = new sf::Music();
	inline static std::vector<sf::Sound*> sounds;
	inline static sf::SoundBuffer* buffer = new sf::SoundBuffer();
};
