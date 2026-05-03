#pragma once
#include "Enemy.h"

class Level {
private:
    int levelNumber;

public:
    Level(int level);
    void initializeEnemy(Enemy& enemy, int difficulty);
};