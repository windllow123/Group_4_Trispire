#pragma once
#include <string>
#include <vector>
#include "Skill.h"

struct Player;

// Contains save slot information for display
struct SaveSlotInfo {
    bool valid = false;
    int difficulty = 0;
    int currentLevel = 0;
    int playerHp = 0;
    int playerMaxHp = 0;
    int skillCount = 0;
};

// Handles saving and loading game state to/from text files
class SaveLoad {
public:
    static constexpr int SLOT_COUNT = 3;

    // Function: Returns the maximum level for a given difficulty
    // Input: difficulty - 0 for easy, 1 for normal, 2 for hard
    // Output: 4 for easy/normal, 5 for hard (includes Boss)
    static int maxLevelForDifficulty(int difficulty);

    // Function: Returns the file path for a save slot
    // Input: slot - the slot number (1-based)
    // Output: the file path string
    static std::string slotFilePath(int slot);

    // Function: Saves game state to a slot file
    // Input: slot - slot number to save to
    // Input: difficulty - current difficulty setting
    // Input: currentLevel - current level number
    // Input: playerHp - current player HP
    // Input: playerMaxHp - maximum player HP
    // Input: skills - list of acquired skills
    // Output: true if save succeeded
    static bool saveGame(int slot, int difficulty, int currentLevel, int playerHp, int playerMaxHp,
                         const std::vector<Skill>& skills);

    // Function: Loads game state from a slot file
    // Input: slot - slot number to load from
    // Input: difficulty - reference to store loaded difficulty
    // Input: currentLevel - reference to store loaded level
    // Input: player - reference to player object to restore
    // Output: true if load succeeded
    static bool loadGame(int slot, int& difficulty, int& currentLevel, Player& player);

    // Function: Reads slot info without fully loading the game
    // Input: slot - slot number to peek
    // Input: info - reference to store slot information
    // Output: true if slot contains valid data
    static bool peekSlot(int slot, SaveSlotInfo& info);

    // Function: Deletes a save slot file
    // Input: slot - slot number to clear
    static void clearSaveSlot(int slot);
};
