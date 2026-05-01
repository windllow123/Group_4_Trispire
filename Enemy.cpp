#include "Enemy.h"
#include <algorithm>
#include <iostream>
#include "ConsoleUtils.h"

Enemy::Enemy() {
    deck.initEnemyDeck();
    startDraw();
}

void Enemy::applyDifficulty(int difficulty) {
    switch (difficulty) {
        case 0:
            max_hp = hp = 3;
            enemy_damage = 1;
            regen_amount = 0;
            break;
        case 1:
            max_hp = hp = 4;
            enemy_damage = 1;
            regen_amount = 1;
            break;
        case 2:
            max_hp = hp = 5;
            enemy_damage = 1;
            regen_amount = 2;
            break;
        default:
            max_hp = hp = 4;
            enemy_damage = 1;
            regen_amount = 1;
            break;
    }
}

void Enemy::regenerate() {
    if (regen_amount > 0 && hp < max_hp) {
        int heal = std::min(regen_amount, max_hp - hp);
        hp += heal;
        std::cout << "敌人回复了" << heal << "点生命！\n";
        sleepMs(1000);
    }
}

void Enemy::startDraw() {
    for (int i = 0; i < 2; i++) hand.push_back(deck.drawCard());
}

void Enemy::drawOne() {
    hand.push_back(deck.drawCard());
}

void Enemy::showStatus() {
    std::cout << "敌人血量：" << hp << "/" << max_hp << " | 敌人手牌：" << hand.size() << "张 | 可抵挡杀次数：" << shan_defense << "\n";
}

void Enemy::takeDamage(int dmg) {
    hp -= dmg;
    if (hp < 0) hp = 0;
    std::cout << "敌人受到" << dmg << "点伤害！\n";
    sleepMs(1000);
}

// 新闪机制：抵消玩家的防御次数
void Enemy::attack(Player& p) {
    std::cout << "\n敌人对你使用【杀】！\n";
    sleepMs(1000);
    
    if (p.respondToAttack()) {
        // 玩家成功躲闪
        return;
    }
    
    p.hp -= enemy_damage;
    std::cout << "你受到" << enemy_damage << "点伤害！\n";
    sleepMs(1000);
    if (p.hasSkill("奸雄")) {
        Card gained = deck.drawCard(); // 简化：获得一张牌
        p.hand.push_back(gained);
        std::cout << "奸雄发动！你获得一张牌：【" << gained.getName() << "】\n";
        sleepMs(1000);
    }
}

void Enemy::discardExcessCards() {
    std::cout << "\n敌人弃牌阶段：\n";
    sleepMs(500);
    if (hand.size() <= static_cast<size_t>(hp)) {
        std::cout << "手牌数量未超过血量上限，无需弃牌。\n";
        sleepMs(500);
        return;
    }
    while (hand.size() > static_cast<size_t>(hp)) {
        Card c = hand.back();
        deck.discardCard(c);
        hand.pop_back();
        std::cout << "敌人弃掉一张牌：【" << c.getName() << "】\n";
        sleepMs(500);
    }
}

void Enemy::playCards() {
    std::cout << "\n敌人出牌阶段：\n";
    sleepMs(500);
    // 敌人不再主动出闪，而是在受到攻击时立即响应
    std::cout << "敌人准备应对你的攻击。\n";
    sleepMs(500);
}

bool Enemy::respondToAttack() {
    for (auto it = hand.begin(); it != hand.end(); ++it) {
        if (it->type == CardType::SHAN) {
            std::cout << "敌人立即使用【闪】来躲避！\n";
            sleepMs(1000);
            deck.discardCard(*it);
            hand.erase(it);
            return true;
        }
    }
    return false;
}