#include "Enemy.h"
#include <algorithm>
#include <iostream>
#include <limits>
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
        std::cout << "Enemy has healed" << heal << "points of health！\n";
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
    std::cout << "Enemy Health：" << hp << "/" << max_hp << " | Enemy Cards：" << hand.size() << "Cards | Number of Dodges: " << shan_defense << "\n";
}

void Enemy::takeDamage(int dmg) {
    hp -= dmg;
    if (hp < 0) hp = 0;
    std::cout << "Enemy recieves " << dmg << " points of damages!\n";
    sleepMs(1000);
}

// 新闪机制：抵消玩家的防御次数
void Enemy::attack(Player& p) {
    turnNumber++;
    std::cout << "\nEnemy used【Strike】against you！\n";
    sleepMs(1000);
    
    if (sleepFirstTurn && turnNumber == 1) {
        std::cout << "Enemy is sleeping this turn.\n";
        sleepMs(1000);
        return;
    }
    
    if (activateUnstoppableFirstTurn && turnNumber == 1) {
        std::cout << "Enemy activates Unstoppable state! All attacks now require 2 Dodges to block!\n";
        sleepMs(1000);
        return;
    }
    
    // Champion only: activate Eight Trigrams Formation on second enemy attack turn
    if (getsEightTrigramsFormation && turnNumber == 2 && !hasEightTrigrams) {
        hasEightTrigrams = true;
        std::cout << "Enemy activates Eight Trigrams Formation!\n";
        sleepMs(1000);
    }

    // Eight Trigrams Formation: 50% chance to gain an extra Dodge each turn
    if (hasEightTrigrams && (rand() % 2 == 0)) {
        hand.push_back(Card(CardType::SHAN));
        std::cout << "Eight Trigrams Formation: Enemy gains an extra Dodge!\n";
        sleepMs(1000);
    }
    
    int requiredShan = unstoppableAlways ? 2 : 1;
    
    if (heavySmashSecondTurn && turnNumber == 2) {
        std::cout << "Heavy Smash!\n";
        sleepMs(1000);
    }
    
    if (p.respondToAttack(requiredShan)) {
        // 玩家成功躲闪
        return;
    }

    if (p.hp <= enemy_damage && p.hasCard(CardType::TOTEM)) {
        int choice = -1;
        while (choice != 0 && choice != 1) {
            std::cout << "\nYour HP would drop to zero. Use The Totem to claim 2 health and survive? (1=yes, 0=no): ";
            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please enter 1 to use The Totem or 0 to take the damage.\n";
                choice = -1;
                continue;
            }
            if (choice != 0 && choice != 1) {
                std::cout << "Invalid input. Please enter 1 to use The Totem or 0 to take the damage.\n";
            }
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (choice == 1) {
            p.removeCard(CardType::TOTEM);
            p.hp += 2;
            if (p.hp > p.max_hp) p.hp = p.max_hp;
            std::cout << "You used The Totem! Claim 2 health and avoid defeat.\n";
            sleepMs(1000);
            return;
        }
        std::cout << "You chose not to use The Totem.\n";
        sleepMs(1000);
    }
    
    p.hp -= enemy_damage;
    std::cout << "You recieved " << enemy_damage << " points of damages!\n";
    sleepMs(1000);
    if (p.hasSkill("Ambition")) {
        Card gained = deck.drawCard(); // 简化：获得一张牌
        p.hand.push_back(gained);
        std::cout << "【Ambition】Triggered! You claim 1 card:【" << gained.getName() << "】\n";
        sleepMs(1000);
    }
}

void Enemy::discardExcessCards() {
    std::cout << "\nEnemy Discarding: \n";
    sleepMs(500);
    if (hand.size() <= static_cast<size_t>(hp)) {
        std::cout << "Hand card number less than max health, no need to discard.\n";
        sleepMs(500);
        return;
    }
    while (hand.size() > static_cast<size_t>(hp)) {
        Card c = hand.back();
        deck.discardCard(c);
        hand.pop_back();
        std::cout << "Enemy discarded 1 card:【" << c.getName() << "】\n";
        sleepMs(500);
    }
}

void Enemy::playCards() {
    std::cout << "\nEnemy's Round\n";
    sleepMs(500);
    // 敌人不再主动出闪，而是在受到攻击时立即响应
    std::cout << "Enemy prepares for your strike\n";
    sleepMs(500);
}

bool Enemy::respondToAttack() {
    for (auto it = hand.begin(); it != hand.end(); ++it) {
        if (it->type == CardType::SHAN) {
            std::cout << "Enemy uses【Dodge】to avoid damage!\n";
            sleepMs(1000);
            deck.discardCard(*it);
            hand.erase(it);
            return true;
        }
    }
    return false;
}
