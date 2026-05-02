#include "SaveLoad.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Skill.h"

const std::string SAVE_FILE = "save.txt";

bool SaveLoad::saveGame(int difficulty, int currentLevel, const Player& player) {
    std::ofstream file(SAVE_FILE);
    if (!file.is_open()) {
        std::cout << "Failed to save game.\n";
        return false;
    }

    file << "difficulty:" << difficulty << "\n";
    file << "currentLevel:" << currentLevel << "\n";
    file << "playerHp:" << player.hp << "\n";
    file << "playerMaxHp:" << player.max_hp << "\n";
    file << "skills:";
    for (size_t i = 0; i < player.skills.size(); ++i) {
        file << player.skills[i].id;
        if (i < player.skills.size() - 1) file << ",";
    }
    file << "\n";

    file.close();
    std::cout << "Game saved successfully.\n";
    return true;
}

bool SaveLoad::loadGame(int& difficulty, int& currentLevel, Player& player) {
    std::ifstream file(SAVE_FILE);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        if (std::getline(iss, key, ':')) {
            std::string value;
            std::getline(iss, value);

            if (key == "difficulty") {
                difficulty = std::stoi(value);
            } else if (key == "currentLevel") {
                currentLevel = std::stoi(value);
            } else if (key == "playerHp") {
                player.hp = std::stoi(value);
            } else if (key == "playerMaxHp") {
                player.max_hp = std::stoi(value);
            } else if (key == "skills") {
                player.skills.clear();
                std::istringstream skillStream(value);
                std::string skillId;
                while (std::getline(skillStream, skillId, ',')) {
                    int id = std::stoi(skillId);
                    Skill s = Skill::getSkillById(id);
                    if (s.id != -1) {
                        player.skills.push_back(s);
                    }
                }
            }
        }
    }

    file.close();
    player.applySkillEffects(); // Reapply skills
    return true;
}

bool SaveLoad::hasSaveFile() {
    std::ifstream file(SAVE_FILE);
    return file.good();
}

void SaveLoad::deleteSaveFile() {
    std::remove(SAVE_FILE.c_str());
}