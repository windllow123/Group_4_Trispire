#pragma once
#include "Enemy.h"

// Handles level-specific enemy configuration and initialization
class Level {
private:
    int levelNumber;

public:
    // Function: Constructs a level with the given level number
    // Input: level - the level number (1-5)
    Level(int level);
    
    // Function: Configures enemy with level-specific stats, abilities, and deck
    // Input: enemy - reference to the enemy to initialize
    // Input: difficulty - difficulty setting (0=easy, 1=normal, 2=hard)
    void initializeEnemy(Enemy& enemy, int difficulty);
};
