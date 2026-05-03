#include "Level.h"
#include <string>

Level::Level(int level) : levelNumber(level) {}

void Level::initializeEnemy(Enemy& enemy, int difficulty) {
    enemy.turnNumber = 0;
    enemy.sleepFirstTurn = false;
    enemy.heavySmashSecondTurn = false;
    enemy.activateUnstoppableFirstTurn = false;
    enemy.unstoppableAlways = false;
    enemy.getsEightTrigramsFormation = false;
    enemy.hasEightTrigrams = false;

    // Base HP based on difficulty
    int baseHp;
    int regen;
    int damage;
    switch (difficulty) {
        case 0: // Easy (menu index)
            baseHp = 3;
            regen = 0;
            damage = 1;
            break;
        case 1: // Normal
            baseHp = 4;
            regen = 1;
            damage = 1;
            break;
        case 2: // Hard
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

    // HP increases with level
    int hpIncrement = 2; // Each level adds 2 HP
    int calculatedHp = baseHp + (levelNumber - 1) * hpIncrement;
    // Boss (level 5, Hard only): double HP — same as original Boss
    if (levelNumber == 5 && difficulty == 2) {
        calculatedHp *= 2;
    }
    enemy.max_hp = enemy.hp = calculatedHp;
    enemy.regen_amount = regen;
    enemy.enemy_damage = (levelNumber >= 3) ? ((difficulty == 2) ? 2 : 1) : damage; // Subsequent enemies: Hard=3, Easy/Normal=2

    // Names: levels 1–4 shared; level 5 is Hard-only Boss
    std::string names[] = {"Unknown Soldier", "Warrior", "Knight", "Champion"};
    if (levelNumber == 5 && difficulty == 2) {
        enemy.name = "Boss";
    } else if (levelNumber >= 1 && levelNumber <= 4) {
        enemy.name = names[levelNumber - 1];
    } else {
        enemy.name = "Enemy";
    }

    // Special abilities for level 2
    if (levelNumber == 2) {
        enemy.activateUnstoppableFirstTurn = true;
        enemy.canUseTao = false; // Though not used, as enemy doesn't play Tao
    }

    // Unstoppable for level 2 and above
    if (levelNumber >= 2) {
        enemy.unstoppableAlways = true;
    }

    // Special abilities for level 3
    if (levelNumber == 3) {
        enemy.sleepFirstTurn = true;
        enemy.heavySmashSecondTurn = true;
    }

    // Special abilities for level 4 (Champion): formation unlocks on 2nd enemy attack turn
    if (levelNumber == 4) {
        enemy.getsEightTrigramsFormation = true;
        enemy.hasEightTrigrams = false;
    }

    // Hard level 5 Boss: no Champion/Knight gimmicks; normal fight with double HP (applied above)
    if (levelNumber == 5 && difficulty == 2) {
        enemy.sleepFirstTurn = false;
        enemy.heavySmashSecondTurn = false;
        enemy.activateUnstoppableFirstTurn = false;
        enemy.getsEightTrigramsFormation = false;
        enemy.hasEightTrigrams = false;
    }

    // Reinitialize deck and hand
    enemy.discardHandToDeck();
    enemy.deck.initEnemyDeck();
    enemy.startDraw();
}
