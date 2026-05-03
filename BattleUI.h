#pragma once

#include <string>
#include <vector>

#include "Enemy.h"
#include "Player.h"

class BattleUI {
public:
    // Function: Adds a message to the battle log
    // Input: message - the log message to add
    void addLog(const std::string& message);

    // Function: Renders the complete battle interface to the console
    // Input: player - current player state with HP, hand, skills
    // Input: enemy - current enemy state with HP, hand, abilities
    // Input: level - current game level number
    // Input: round - current battle round number
    // Input: selectedCard - index of currently selected card in hand
    // Input: phase - current battle phase description
    // Input: helper - helper instruction text at bottom of screen
    void render(const Player& player,
                const Enemy& enemy,
                int level,
                int round,
                int selectedCard,
                const std::string& phase,
                const std::string& helper) const;

private:
    std::vector<std::string> logs;
    const int maxLogs = 250;
};
