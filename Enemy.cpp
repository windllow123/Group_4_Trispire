#include "Enemy.h"
#include <algorithm>
#include <iostream>
#include <string>
#include "Player.h"
#include "ConsoleUtils.h"
#include "BattleUI.h"

using namespace std;

// Function: Constructs enemy, initializes deck with enemy cards and draws hand
Enemy::Enemy() {
    deck.initEnemyDeck();
    startDraw();
}

// Function: Destroys enemy, returns all hand cards to deck
Enemy::~Enemy() {
    discardHandToDeck();
}

// Function: Sets enemy base stats based on selected difficulty
// Input: difficulty - 0 for easy (HP 3, no regen), 1 for normal (HP 4, regen 1), 2 for hard (HP 5, regen 2)
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

// Function: Regenerates enemy health by regen_amount if not at max HP
void Enemy::regenerate() {
    if (regen_amount > 0 && hp < max_hp) {
        int heal = min(regen_amount, max_hp - hp);
        hp += heal;
        cout << "Enemy has healed" << heal << "points of health！\n";
        sleepMs(1000);
    }
}

// Function: Draws initial 2 cards for enemy's starting hand
void Enemy::startDraw() {
    for (int i = 0; i < 2; i++) {
        Card* card = deck.drawCard();
        if (card != nullptr) {
            hand.push_back(card);
        }
    }
}

// Function: Draws 1 card from deck and adds to hand
void Enemy::drawOne() {
    Card* card = deck.drawCard();
    if (card != nullptr) {
        hand.push_back(card);
    }
}

// Function: Returns all hand cards to the discard pile
void Enemy::discardHandToDeck() {
    for (Card* card : hand) {
        deck.discardCard(card);
    }
    hand.clear();
}

// Function: Displays enemy's current HP and hand size
void Enemy::showStatus() {
    cout << "Enemy Health：" << hp << "/" << max_hp << " | Enemy Cards：" << hand.size() << "Cards | Number of Dodges: " << shan_defense << "\n";
}

// Function: Applies damage to enemy, cannot go below 0
// Input: dmg - amount of damage to deal
void Enemy::takeDamage(int dmg) {
    hp -= dmg;
    if (hp < 0) hp = 0;
    cout << "Enemy recieves " << dmg << " points of damages!\n";
    sleepMs(1000);
}

// Function: Executes enemy's full attack sequence with special abilities
// Input: p - reference to player being attacked
// Input: ui - reference to battle UI for logging
// Output: true if attack action was performed
bool Enemy::attack(Player& p, BattleUI& ui) {
    turnNumber++;
    
    if (sleepFirstTurn && turnNumber == 1) {
        cout << "Enemy is sleeping this turn.\n";
        sleepMs(1000);
        return false;
    }
    
    if (activateUnstoppableFirstTurn && turnNumber == 1) {
        cout << "Enemy activates Unstoppable state! All attacks now require 2 Dodges to block!\n";
        sleepMs(1000);
        return false;
    }
    
    cout << "\nEnemy used【Strike】against you！\n";
    sleepMs(1000);
    
    if (getsEightTrigramsFormation && turnNumber == 2 && !hasEightTrigrams) {
        hasEightTrigrams = true;
        cout << "Enemy activates Eight Trigrams Formation!\n";
        sleepMs(1000);
    }

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
        return true;
    }

    if (p.hp <= enemy_damage && p.hasCard(CardType::TOTEM)) {
        int choice = -1;
        std::string prompt = "Your HP would drop to zero. Use The Totem to claim 2 health and survive? (1=yes, 0=no): ";
        bool first = true;
        while (choice != 0 && choice != 1) {
            if (first) {
                cout << "\n" << prompt;
                first = false;
            }
            int key = getKey();
            if (key == '1') {
                choice = 1;
                cout << "1\n";
            } else if (key == '0') {
                choice = 0;
                cout << "0\n";
            } else {
                cout << "\rInvalid input. Please enter 1 to use The Totem or 0 to take the damage.";
                sleepMs(1000);
                cout << "\r" << prompt;
            }
        }
        if (choice == 1) {
            clearScreen();
            cout << "By the power of The Totem, you are rejuvenated!\n";
            cout << "You gain 2 health and survive the fatal blow!\n";
            sleepMs(2000);
            p.removeCard(CardType::TOTEM);
            p.hp += 2;
            if (p.hp > p.max_hp) p.hp = p.max_hp;
            ui.addLog("The Totem shattered and granted you 2 HP, saving you from defeat!");
            return true;
        }
        cout << "You chose not to use The Totem.\n";
        sleepMs(1000);
    }
    
    p.hp -= enemy_damage;
    cout << "You recieved " << enemy_damage << " points of damages!\n";
    sleepMs(1000);
    if (p.hasSkill("Ambition")) {
        for (int i = 0; i < 2; ++i) {
            Card* gained = deck.drawCard();
            if (gained != nullptr) {
                p.hand.push_back(gained);
            }
        }
        p.enforceHandLimit();
        cout << "【Ambition】Triggered! You claim 2 cards.\n";
        sleepMs(1000);
    }
    return true;
}

// Function: Discards random cards from hand until size does not exceed HP
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

// Function: Enemy prepares for player's strike, announces turn
void Enemy::playCards() {
    cout << "\nEnemy's Round\n";
    sleepMs(500);
    cout << "Enemy prepares for your strike\n";
    sleepMs(500);
}

// Function: Searches hand for a Dodge card to avoid incoming attack
// Output: true if dodge card was found and used, false otherwise
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
