#include "Level.h"
#include <string>

// Function: Constructs a level with given number
// Input: level - the level index (1-based)
Level::Level(int level) : levelNumber(level) {}

// Function: Sets all enemy properties based on level and difficulty
// Configures HP, damage, regeneration, name, and special abilities
// Input: enemy - reference to enemy object to configure
// Input: difficulty - 0 for easy, 1 for normal, 2 for hard
void Level::initializeEnemy(Enemy& enemy, int difficulty) {
    enemy.turnNumber = 0;
    enemy.sleepFirstTurn = false;
    enemy.heavySmashSecondTurn = false;
    enemy.activateUnstoppableFirstTurn = false;
    enemy.unstoppableAlways = false;
    enemy.getsEightTrigramsFormation = false;
    enemy.hasEightTrigrams = false;

    int baseHp;
    int regen;
    int damage;
    switch (difficulty) {
        case 0:
            baseHp = 3;
            regen = 0;
            damage = 1;
            break;
        case 1:
            baseHp = 4;
            regen = 1;
            damage = 1;
            break;
        case 2:
            baseHp = 5;
            regen = 2;
            damage = 1;
            break;
        default:
            baseHp = 4;
            regen = 1;
            damage = 1;
            break;
    }

    int hpIncrement = 2;
    int calculatedHp = baseHp + (levelNumber - 1) * hpIncrement;
    if (levelNumber == 5 && difficulty == 2) {
        calculatedHp *= 2;
    }
    enemy.max_hp = enemy.hp = calculatedHp;
    enemy.regen_amount = regen;
    enemy.enemy_damage = (levelNumber >= 3) ? ((difficulty == 2) ? 2 : 1) : damage;

    std::string names[] = {"Unknown Soldier", "Warrior", "Knight", "Champion"};
    if (levelNumber == 5 && difficulty == 2) {
        enemy.name = "Boss";
    } else if (levelNumber >= 1 && levelNumber <= 4) {
        enemy.name = names[levelNumber - 1];
    } else {
        enemy.name = "Enemy";
    }

    if (levelNumber == 2) {
        enemy.activateUnstoppableFirstTurn = true;
        enemy.canUseTao = false;
    }

    if (levelNumber >= 2) {
        enemy.unstoppableAlways = true;
    }

    if (levelNumber == 3) {
        enemy.sleepFirstTurn = true;
        enemy.heavySmashSecondTurn = true;
    }

    if (levelNumber == 4) {
        enemy.getsEightTrigramsFormation = true;
        enemy.hasEightTrigrams = false;
    }

    if (levelNumber == 5 && difficulty == 2) {
        enemy.sleepFirstTurn = false;
        enemy.heavySmashSecondTurn = false;
        enemy.activateUnstoppableFirstTurn = false;
        enemy.getsEightTrigramsFormation = false;
        enemy.hasEightTrigrams = false;
    }

    enemy.discardHandToDeck();
    enemy.deck.initEnemyDeck();
    enemy.startDraw();
}
