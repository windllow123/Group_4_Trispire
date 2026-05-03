#pragma once

#include <string>
#include <vector>

#include "Enemy.h"
#include "Player.h"

class BattleUI {
public:
    void addLog(const std::string& message);

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
