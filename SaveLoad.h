#pragma once
#include <string>
#include <vector>
#include "Skill.h"

struct Player;

struct SaveSlotInfo {
    bool valid = false;
    int difficulty = 0;
    int currentLevel = 0;
    int playerHp = 0;
    int playerMaxHp = 0;
    int skillCount = 0;
};

class SaveLoad {
public:
    static constexpr int SLOT_COUNT = 3;

    /** Easy/Normal: 4 levels. Hard: 5 levels (extra Boss). */
    static int maxLevelForDifficulty(int difficulty);

    static std::string slotFilePath(int slot);

    static bool saveGame(int slot, int difficulty, int currentLevel, int playerHp, int playerMaxHp,
                         const std::vector<Skill>& skills);
    static bool loadGame(int slot, int& difficulty, int& currentLevel, Player& player);
    static bool peekSlot(int slot, SaveSlotInfo& info);
    static void clearSaveSlot(int slot);
};
