#pragma once
#include <vector>
#include <string>
#include "Card.h"
#include "Player.h"
#include "Deck.h"

struct Enemy {
    int hp = 3;
    int max_hp = 3;
    std::string name = "小兵";
    std::vector<Card> hand;
    Deck deck;
    int shan_defense = 0;
    int enemy_damage = 1;
    int regen_amount = 0;

    Enemy();
    void startDraw();
    void drawOne();
    void showStatus();
    void takeDamage(int dmg);
    void attack(Player& p);
    void discardExcessCards();
    void playCards();
    void applyDifficulty(int difficulty);
    void regenerate();
    bool respondToAttack();
};