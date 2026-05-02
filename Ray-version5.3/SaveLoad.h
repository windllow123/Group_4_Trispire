#pragma once
#include <string>
#include "Player.h"

class SaveLoad {
public:
    static bool saveGame(int difficulty, int currentLevel, const Player& player);
    static bool loadGame(int& difficulty, int& currentLevel, Player& player);
    static bool hasSaveFile();
    static void deleteSaveFile();
};