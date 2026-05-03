#include "Enemy.h"
#include <algorithm>
#include <iostream>
#include "Player.h"
#include "ConsoleUtils.h"

using namespace std;

Enemy::Enemy() {
    deck.initEnemyDeck();
    startDraw();
}

Enemy::~Enemy() {
    discardHandToDeck();
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
        int heal = min(regen_amount, max_hp - hp);
        hp += heal;
        cout << "Enemy has healed" << heal << "points of health！\n";
        sleepMs(1000);
    }
}

void Enemy::startDraw() {
    for (int i = 0; i < 2; i++) {
        Card* card = deck.drawCard();
        if (card != nullptr) {
            hand.push_back(card);
        }
    }
}

void Enemy::drawOne() {
    Card* card = deck.drawCard();
    if (card != nullptr) {
        hand.push_back(card);
    }
}

void Enemy::discardHandToDeck() {
    for (Card* card : hand) {
        deck.discardCard(card);
    }
    hand.clear();
}

void Enemy::showStatus() {
    cout << "Enemy Health：" << hp << "/" << max_hp << " | Enemy Cards：" << hand.size() << "Cards | Number of Dodges: " << shan_defense << "\n";
}

void Enemy::takeDamage(int dmg) {
    hp -= dmg;
    if (hp < 0) hp = 0;
    cout << "Enemy recieves " << dmg << " points of damages!\n";
    sleepMs(1000);
}

// 新闪机制：抵消玩家的防御次数
void Enemy::attack(Player& p) {
    turnNumber++;
    cout << "\nEnemy used【Strike】against you！\n";
    sleepMs(1000);
    
    if (sleepFirstTurn && turnNumber == 1) {
        cout << "Enemy is sleeping this turn.\n";
        sleepMs(1000);
        return;
    }
    
    if (activateUnstoppableFirstTurn && turnNumber == 1) {
        cout << "Enemy activates Unstoppable state! All attacks now require 2 Dodges to block!\n";
        sleepMs(1000);
        return;
    }
    
    // Champion only: activate Eight Trigrams Formation on second enemy attack turn
    if (getsEightTrigramsFormation && turnNumber == 2 && !hasEightTrigrams) {
        hasEightTrigrams = true;
        cout << "Enemy activates Eight Trigrams Formation!\n";
        sleepMs(1000);
    }

    // Eight Trigrams Formation: 50% chance to gain an extra Dodge each turn
    if (hasEightTrigrams && (rand() % 2 == 0)) {
        hand.push_back(g_cardPool.acquire(CardType::SHAN));
        cout << "Eight Trigrams Formation: Enemy gains an extra Dodge!\n";
        sleepMs(1000);
    }
    
    int requiredShan = unstoppableAlways ? 2 : 1;
    
    if (heavySmashSecondTurn && turnNumber == 2) {
        cout << "Heavy Smash!\n";
        sleepMs(1000);
    }
    
    if (p.respondToAttack(requiredShan)) {
        // 玩家成功躲闪
        return;
    }

    if (p.hp <= enemy_damage && p.hasCard(CardType::TOTEM)) {
        int choice = -1;
        while (choice != 0 && choice != 1) {
            cout << "\nYour HP would drop to zero. Use The Totem to claim 2 health and survive? (1=yes, 0=no): ";
            int key = getKey();
            if (key == '1') {
                choice = 1;
                cout << "1\n";
            } else if (key == '0') {
                choice = 0;
                cout << "0\n";
            } else {
                cout << "Invalid input. Please enter 1 to use The Totem or 0 to take the damage.\n";
            }
        }
        if (choice == 1) {
            p.removeCard(CardType::TOTEM);
            p.hp += 2;
            if (p.hp > p.max_hp) p.hp = p.max_hp;
            cout << "You used The Totem! Claim 2 health and avoid defeat.\n";
            sleepMs(1000);
            return;
        }
        cout << "You chose not to use The Totem.\n";
        sleepMs(1000);
    }
    
    p.hp -= enemy_damage;
    cout << "You recieved " << enemy_damage << " points of damages!\n";
    sleepMs(1000);
    if (p.hasSkill("Ambition")) {
        Card* gained = deck.drawCard(); // 简化：获得一张牌
        if (gained != nullptr) {
            p.hand.push_back(gained);
            cout << "【Ambition】Triggered! You claim 1 card:【" << gained->getName() << "】\n";
            sleepMs(1000);
        }
    }
}

void Enemy::discardExcessCards() {
    cout << "\nEnemy Discarding: \n";
    sleepMs(500);
    if (hand.size() <= static_cast<size_t>(hp)) {
        cout << "Hand card number less than max health, no need to discard.\n";
        sleepMs(500);
        return;
    }
    while (hand.size() > static_cast<size_t>(hp)) {
        Card* c = hand.back();
        deck.discardCard(c);
        hand.pop_back();
        if (c != nullptr) {
            cout << "Enemy discarded 1 card:【" << c->getName() << "】\n";
        } else {
            cout << "Enemy discarded 1 card.\n";
        }
        sleepMs(500);
    }
}

void Enemy::playCards() {
    cout << "\nEnemy's Round\n";
    sleepMs(500);
    // 敌人不再主动出闪，而是在受到攻击时立即响应
    cout << "Enemy prepares for your strike\n";
    sleepMs(500);
}

bool Enemy::respondToAttack() {
    for (auto it = hand.begin(); it != hand.end(); ++it) {
        if (*it != nullptr && (*it)->type == CardType::SHAN) {
            cout << "Enemy uses【Dodge】to avoid damage!\n";
            sleepMs(1000);
            deck.discardCard(*it);
            hand.erase(it);
            return true;
        }
    }
    return false;
}
