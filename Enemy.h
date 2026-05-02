#pragma once
#include <vector>
#include <string>
#include "Card.h"
#include "Player.h"
#include "Deck.h"

struct Enemy {
    int hp = 3;
    int max_hp = 3;
    std::string name = "Soldier";
    std::vector<Card*> hand;
    Deck deck;
    int shan_defense = 0;
    int enemy_damage = 1;
    int regen_amount = 0;
    bool canUseTao = true;
    bool sleepFirstTurn = false;
    bool heavySmashSecondTurn = false;
    int turnNumber = 0;
    bool activateUnstoppableFirstTurn = false;
    bool unstoppableAlways = false;
    /** Only Champion (level 4): formation unlocks on enemy attack turn 2. */
    bool getsEightTrigramsFormation = false;
    bool hasEightTrigrams = false;

    Enemy();
    void startDraw();
    void drawOne();
    void discardHandToDeck();
    void showStatus();
    void takeDamage(int dmg);
    void attack(Player& p);
    void discardExcessCards();
    void playCards();
    void applyDifficulty(int difficulty);
    void regenerate();
    bool respondToAttack();
};
