#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <array>
#include <tuple>

class Score
{
public:
	inline static int score = 0;
	inline static std::array<std::tuple<std::array<char, 3>, int>, 10> highscores;

	inline static void Add(int amount) { score += amount; }
	inline static void Reset() { score = 0; }
	inline static void LoadHighscores() {
		highscores = std::array<std::tuple<std::array<char, 3>, int>, 10>();
		std::filesystem::path cwd = std::filesystem::current_path();
		std::ifstream file(cwd.string() + "/highscores.sav", std::ios::binary);
		if (!file.is_open()) {
			return;
		}
		for (auto& entry : highscores) {
			file.read(reinterpret_cast<char*>(&entry), sizeof(entry));
		}
		file.close();
	}
	inline static void SaveHighscores() {
		std::filesystem::path cwd = std::filesystem::current_path();
		std::ofstream file(cwd.string() + "/highscores.sav", std::ios::binary);
		if (!file.is_open()) {
			return;
		}
		for (auto& entry : highscores) {
			file.write(reinterpret_cast<char*>(&entry), sizeof(entry));
		}
		file.close();
	}
	inline static void SortHighscores() {
		std::sort(highscores.begin(), highscores.end(), [](const auto& a, const auto& b) {
			return std::get<1>(a) > std::get<1>(b);
			});
	}
	inline static void AddHighscore(const char initials[3]) {
		if (score > std::get<1>(highscores.back())) {
			// Overwrite the lowest score
			std::copy(initials, initials + 3, std::get<0>(highscores.back()).begin());
			std::get<1>(highscores.back()) = score;
			SortHighscores();
		}
	}
};