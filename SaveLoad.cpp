#include "SaveLoad.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Player.h"

namespace {
bool tryParseInt(const std::string& text, int& value) {
    std::istringstream iss(text);
    if (!(iss >> value)) return false;
    char extra;
    return !(iss >> extra);
}
} // namespace

int SaveLoad::maxLevelForDifficulty(int difficulty) {
    return (difficulty == 2) ? 5 : 4;
}

std::string SaveLoad::slotFilePath(int slot) {
    return "save_slot_" + std::to_string(slot) + ".txt";
}

bool SaveLoad::saveGame(int slot, int difficulty, int currentLevel, int playerHp, int playerMaxHp,
                        const std::vector<Skill>& skills) {
    if (slot < 1 || slot > SLOT_COUNT) {
        std::cout << "Invalid save slot.\n";
        return false;
    }
    std::ofstream file(slotFilePath(slot));
    if (!file.is_open()) {
        std::cout << "Failed to save game.\n";
        return false;
    }

    file << "difficulty:" << difficulty << "\n";
    file << "currentLevel:" << currentLevel << "\n";
    file << "playerHp:" << playerHp << "\n";
    file << "playerMaxHp:" << playerMaxHp << "\n";
    file << "skills:";
    for (size_t i = 0; i < skills.size(); ++i) {
        file << skills[i].id;
        if (i < skills.size() - 1) file << ",";
    }
    file << "\n";

    file.close();
    std::cout << "Game saved to slot " << slot << ".\n";
    return true;
}

bool SaveLoad::loadGame(int slot, int& difficulty, int& currentLevel, Player& player) {
    if (slot < 1 || slot > SLOT_COUNT) {
        return false;
    }
    std::ifstream file(slotFilePath(slot));
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
    {
        int maxLv = maxLevelForDifficulty(loadedDifficulty);
        if (loadedLevel < 1 || loadedLevel > maxLv) {
            return false;
        }
    }
    if (loadedMaxHp <= 0 || loadedMaxHp > 100) {
        return false;
    }
    if (loadedHp < 1 || loadedHp > loadedMaxHp) {
        return false;
    }

    difficulty = loadedDifficulty;
    currentLevel = loadedLevel;
    player.max_hp = loadedMaxHp;
    player.hp = loadedHp;
    player.skills = loadedSkills;
    player.applySkillEffects();
    return true;
}

bool SaveLoad::peekSlot(int slot, SaveSlotInfo& info) {
    info = SaveSlotInfo{};
    if (slot < 1 || slot > SLOT_COUNT) {
        return false;
    }
    std::ifstream file(slotFilePath(slot));
    if (!file.is_open()) {
        return false;
    }

    int loadedDifficulty = -1;
    int loadedLevel = -1;
    int loadedHp = -1;
    int loadedMaxHp = -1;
    size_t skillCount = 0;

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
                skillCount = 0;
                std::istringstream skillStream(value);
                std::string skillId;
                while (std::getline(skillStream, skillId, ',')) {
                    if (skillId.empty()) continue;
                    int id = -1;
                    if (!tryParseInt(skillId, id)) return false;
                    if (Skill::getSkillById(id).id != -1) {
                        skillCount++;
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
    if (loadedLevel < 1 || loadedLevel > maxLevelForDifficulty(loadedDifficulty)) {
        return false;
    }
    if (loadedMaxHp <= 0 || loadedHp < 1 || loadedHp > loadedMaxHp) {
        return false;
    }

    info.valid = true;
    info.difficulty = loadedDifficulty;
    info.currentLevel = loadedLevel;
    info.playerHp = loadedHp;
    info.playerMaxHp = loadedMaxHp;
    info.skillCount = static_cast<int>(skillCount);
    return true;
}

void SaveLoad::clearSaveSlot(int slot) {
    if (slot < 1 || slot > SLOT_COUNT) {
        return;
    }
    std::remove(slotFilePath(slot).c_str());
}
