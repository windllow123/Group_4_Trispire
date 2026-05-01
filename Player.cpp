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
    std::cout << "\n你的手牌：\n";
    for (size_t i = 0; i < hand.size(); i++) {
        std::cout << i << " - 【" << hand[i].getName() << "】\n";
    }
    std::cout << "本回合剩余出杀次数：" << max_sha_per_turn - current_sha_used << "\n";
}

void Player::showStatus() {
    std::cout << "玩家血量：" << hp << "/" << max_hp << " | 总手牌：" << hand.size() << "张\n";
}

void Player::addSkill(const Skill& s) {
    skills.push_back(s);
    applySkillEffects();
    std::cout << "获得技能：【" << s.name << "】" << s.desc << "\n";
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
    if (hasSkill("咆哮")) {
        max_sha_per_turn += 1;
    }
}

void Player::useSkillKuRou() {
    if (hp > 1 && hasSkill("苦肉")) {
        hp -= 1;
        std::cout << "使用【苦肉】，扣除1点血量，获得两张牌！\n";
        sleepMs(1000);
        drawTwo();
    } else {
        std::cout << "无法使用苦肉（血量不足或无此技能）。\n";
        sleepMs(1000);
    }
}

void Player::discardExcessCards() {
    std::cout << "\n玩家弃牌阶段：\n";
    sleepMs(500);
    if (hasSkill("克己") && current_sha_used == 0) {
        std::cout << "克己发动！跳过弃牌阶段。\n";
        sleepMs(1000);
        return;
    }
    if (static_cast<int>(hand.size()) <= hp) {
        std::cout << "手牌数量未超过血量上限，无需弃牌。\n";
        sleepMs(500);
        return;
    }
    while (static_cast<int>(hand.size()) > hp) {
        std::cout << "\n手牌超过血量上限，请选择弃掉的牌：\n";
        showHand();
        std::cout << "输入牌索引：";
        int discard_idx;
        std::cin >> discard_idx;
        if (discard_idx >= 0 && discard_idx < static_cast<int>(hand.size())) {
            Card c = hand[discard_idx];
            deck.discardCard(c);
            hand.erase(hand.begin() + discard_idx);
            std::cout << "弃掉【" << c.getName() << "】\n";
            sleepMs(500);
        } else {
            std::cout << "输入无效！\n";
        }
    }
}

bool Player::respondToAttack() {
    // Check for dodge cards in hand
    for (size_t i = 0; i < hand.size(); i++) {
        if (hand[i].type == CardType::SHAN) {
            std::cout << "\n你有闪卡！是否现在使用来躲避？(0-" << (hand.size() - 1) << ")或-1不躲避: ";
            int response_idx;
            std::cin >> response_idx;
            
            if (response_idx >= 0 && response_idx < static_cast<int>(hand.size()) && 
                hand[response_idx].type == CardType::SHAN) {
                std::cout << "你立即使用【闪】躲避敌人的攻击！\n";
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