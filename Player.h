#pragma once
#include <vector>
#include "Card.h"
#include "Skill.h"
#include "Deck.h"

struct Player {
    int hp = 5;
    int max_hp = 5;
    // 杀限制：每回合2次
    int max_sha_per_turn = 2;
    int current_sha_used = 0;
    // 闪防御次数
    int shan_defense = 0;
    double shan_effectiveness = 1.0;
    double dodge_chance = 0.0;

    std::vector<Card> hand;
    std::vector<Skill> skills;
    Deck deck;

    Player();
    void startDraw();   // 开局抽3张
    void drawTwo();     // 每回合抽2张
    void resetShaCount(); // 回合开始重置杀次数
    void showHand();
    void showStatus();
    void addSkill(const Skill& s);
    bool hasSkill(const std::string& skillName) const;
    void applySkillEffects();
    void applyDifficulty(int difficulty);
    bool useSkillKuRou();
    void discardExcessCards();
    bool respondToAttack(int requiredShan = 1);
};