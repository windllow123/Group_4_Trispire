#include "SaveLoad.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Skill.h"

const std::string SAVE_FILE = "save.txt";

namespace {
bool tryParseInt(const std::string& text, int& value) {
    std::istringstream iss(text);
    if (!(iss >> value)) return false;
    char extra;
    return !(iss >> extra);
}
} // namespace

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

    int loadedDifficulty = -1;
    int loadedLevel = -1;
    int loadedHp = -1;
    int loadedMaxHp = -1;
    std::vector<Skill> loadedSkills;

    bool hasDifficulty = false;
    bool hasLevel = false;
    bool hasHp = false;
    bool hasMaxHp = false;
    bool hasSkills = false;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        if (std::getline(iss, key, ':')) {
            std::string value;
            std::getline(iss, value);

            if (key == "difficulty") {
                if (!tryParseInt(value, loadedDifficulty)) return false;
                hasDifficulty = true;
            } else if (key == "currentLevel") {
                if (!tryParseInt(value, loadedLevel)) return false;
                hasLevel = true;
            } else if (key == "playerHp") {
                if (!tryParseInt(value, loadedHp)) return false;
                hasHp = true;
            } else if (key == "playerMaxHp") {
                if (!tryParseInt(value, loadedMaxHp)) return false;
                hasMaxHp = true;
            } else if (key == "skills") {
                loadedSkills.clear();
                std::istringstream skillStream(value);
                std::string skillId;
                while (std::getline(skillStream, skillId, ',')) {
                    if (skillId.empty()) continue;
                    int id = -1;
                    if (!tryParseInt(skillId, id)) return false;
                    Skill s = Skill::getSkillById(id);
                    if (s.id != -1) {
                        loadedSkills.push_back(s);
                    }
                }
                hasSkills = true;
            }
        }
    }

    file.close();

    if (!hasDifficulty || !hasLevel || !hasHp || !hasMaxHp || !hasSkills) {
        return false;
    }
    if (loadedDifficulty < 0 || loadedDifficulty > 2) {
        return false;
    }
    if (loadedLevel < 1 || loadedLevel > 5) {
        return false;
    }
    if (loadedMaxHp <= 0 || loadedMaxHp > 100) {
        return false;
    }
    if (loadedHp < 0 || loadedHp > loadedMaxHp) {
        return false;
    }

    difficulty = loadedDifficulty;
    currentLevel = loadedLevel;
    player.max_hp = loadedMaxHp;
    player.hp = loadedHp;
    player.skills = loadedSkills;
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
