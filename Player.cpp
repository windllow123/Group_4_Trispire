#include "Player.h"
#include <iostream>
#include "ConsoleUtils.h"

Player::Player() {
    deck.initPlayerDeck();
    startDraw();
}

void Player::applyDifficulty(int difficulty) {
    switch (difficulty) {
        case 0:
            max_hp = hp = 8;
            shan_effectiveness = 1.2;
            dodge_chance = 0.25;
            break;
        case 1:
            max_hp = hp = 6;
            shan_effectiveness = 1.0;
            dodge_chance = 0.15;
            break;
        case 2:
            max_hp = hp = 5;
            shan_effectiveness = 0.6;
            dodge_chance = 0.05;
            break;
        default:
            max_hp = hp = 6;
            shan_effectiveness = 1.0;
            dodge_chance = 0.15;
            break;
    }
    applySkillEffects();
}

// 开局抽3张牌
void Player::startDraw() {
    for (int i = 0; i < 3; i++) hand.push_back(deck.drawCard());
}

void Player::drawTwo() {
    for (int i = 0; i < 2; i++) hand.push_back(deck.drawCard());
}

// 重置杀的使用次数
void Player::resetShaCount() {
    current_sha_used = 0;
}

void Player::showHand() {
    std::cout << "\nYour Handcards：\n";
    for (size_t i = 0; i < hand.size(); i++) {
        std::cout << i << " - 【" << hand[i].getName() << "】\n";
    }
    std::cout << "Strike chances left：" << max_sha_per_turn - current_sha_used << "\n";
}

void Player::showStatus() {
    std::cout << "Player Health" << hp << "/" << max_hp << " | Handcards left" << hand.size() << "\n";
}

void Player::addSkill(const Skill& s) {
    skills.push_back(s);
    applySkillEffects();
    std::cout << "Acquired skill【" << s.name << "】" << s.desc << "\n";
    sleepMs(1000);
}

bool Player::hasSkill(const std::string& skillName) const {
    for (const auto& skill : skills) {
        if (skill.name == skillName) return true;
    }
    return false;
}

void Player::applySkillEffects() {
    max_sha_per_turn = 2; // 默认
    if (hasSkill("Warscream")) {
        max_sha_per_turn += 1;
    }
}

void Player::useSkillKuRou() {
    if (hp > 1 && hasSkill("Sacrifice")) {
        hp -= 1;
        std::cout << "Using【Sacrifice】，Lost 1 point of health，Aquire 2 cards!\n";
        sleepMs(1000);
        drawTwo();
    } else {
        std::cout << "Can't use 【Sacrifice】（not enough health for this skill）\n";
        sleepMs(1000);
    }
}

void Player::discardExcessCards() {
    std::cout << "\nPlay Discarding:\n";
    sleepMs(500);
    if (hasSkill("Temperance") && current_sha_used == 0) {
        std::cout << "【Temperance】Triggered！Skipping discard session!\n";
        sleepMs(1000);
        return;
    }
    if (static_cast<int>(hand.size()) <= hp) {
        std::cout << "Handcard less than max health，no need for discard\n";
        sleepMs(500);
        return;
    }
    while (static_cast<int>(hand.size()) > hp) {
        std::cout << "\nHandcard exceeds max health, choose card to lose: \n";
        showHand();
        std::cout << "Enter card index";
        int discard_idx;
        std::cin >> discard_idx;
        if (discard_idx >= 0 && discard_idx < static_cast<int>(hand.size())) {
            Card c = hand[discard_idx];
            deck.discardCard(c);
            hand.erase(hand.begin() + discard_idx);
            std::cout << "Discarded【" << c.getName() << "】\n";
            sleepMs(500);
        } else {
            std::cout << "Invalid Input!\n";
        }
    }
}

bool Player::respondToAttack() {
    // Check for dodge cards in hand
    for (size_t i = 0; i < hand.size(); i++) {
        if (hand[i].type == CardType::SHAN) {
            std::cout << "\nYou have 【Dodge】！Use it to avoid damage？(0-" << (hand.size() - 1) << ")or -1 to take damage: ";
            int response_idx;
            std::cin >> response_idx;
            
            if (response_idx >= 0 && response_idx < static_cast<int>(hand.size()) && 
                hand[response_idx].type == CardType::SHAN) {
                std::cout << "You used【Dodge】to avoid damage from enemy！\n";
                sleepMs(1000);
                deck.discardCard(hand[response_idx]);
                hand.erase(hand.begin() + response_idx);
                return true;
            }
            break;
        }
    }
    return false;
}
