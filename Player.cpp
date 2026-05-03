#include "Player.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include "ConsoleUtils.h"

static bool tryParseInt(const std::string& input, int& value) {
    std::istringstream iss(input);
    if (!(iss >> value)) return false;
    char extra;
    return !(iss >> extra);
}

Player::Player() {
    deck.initPlayerDeck();
    startDraw();
}

Player::~Player() {
    discardHandToDeck();
}

void Player::discardHandToDeck() {
    for (Card* card : hand) {
        deck.discardCard(card);
    }
    hand.clear();
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

void Player::resetDeckAndHandForLevelEntry() {
    discardHandToDeck();
    deck.initPlayerDeck();
    current_sha_used = 0;
    nextStrikeBonusDamage = 0;
    nextStrikeIgnoreDodge = false;
    startDraw();
}

// 开局抽3张牌
void Player::startDraw() {
    for (int i = 0; i < 3; i++) {
        Card* card = deck.drawCard();
        if (card != nullptr) {
            hand.push_back(card);
        }
    }
    enforceHandLimit();
}

void Player::drawTwo() {
    for (int i = 0; i < 2; i++) {
        Card* card = deck.drawCard();
        if (card != nullptr) {
            hand.push_back(card);
        }
    }
    enforceHandLimit();
}

void Player::drawThree() {
    for (int i = 0; i < 3; i++) {
        Card* card = deck.drawCard();
        if (card != nullptr) {
            hand.push_back(card);
        }
    }
    enforceHandLimit();
}

// 重置杀的使用次数
void Player::resetShaCount() {
    current_sha_used = 0;
}

void Player::showHand() {
    std::cout << "\nYour Handcards：\n";
    for (size_t i = 0; i < hand.size(); i++) {
        std::cout << i << " - 【" << hand[i]->getName() << "】\n";
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
        max_sha_per_turn += 2;
    }
}

bool Player::useSkillKuRou() {
    if (hand.size() > static_cast<size_t>(MAX_HAND_SIZE - 3)) {
        std::cout << "Cannot use【Sacrifice】：need room for 3 cards (hand too full).\n";
        sleepMs(1000);
        return false;
    }
    if (hp > 1 && hasSkill("Sacrifice")) {
        hp -= 1;
        std::cout << "Using【Sacrifice】，Lost 1 point of health，Aquire 3 cards!\n";
        sleepMs(1000);
        drawThree();
        enforceHandLimit();
        return true;
    }
    if (hasSkill("Sacrifice")) {
        std::cout << "Cannot perform this action (insufficient health)!\n";
    } else {
        std::cout << "You don't have this skill!\n";
    }
    sleepMs(1000);
    return false;
}

void Player::enforceHandLimit() {
    while (hand.size() > MAX_HAND_SIZE) {
        Card* c = hand.back();
        deck.discardCard(c);
        hand.pop_back();
        std::cout << "Hand exceeds " << MAX_HAND_SIZE << " cards, automatically discarded【" << c->getName() << "】\n";
        sleepMs(500);
    }
}

bool Player::discardExcessCards(Enemy& enemy, bool& returnToLobby) {
    returnToLobby = false;
    std::cout << "\nPlay Discarding:\n";
    sleepMs(500);
    if (static_cast<int>(hand.size()) <= hp) {
        std::cout << "Handcard less than max health，no need for discard\n";
        sleepMs(500);
        return false;
    }
    while (static_cast<int>(hand.size()) > hp) {
        clearScreen();
        displayHealthBar(*this, enemy);
        std::cout << "\nHandcard exceeds max health, choose card to lose: \n";
        showHand();
        std::cout << "Enter card index: ";
        std::string line;
        bool paused = false;
        if (!readLineWithPauseSupport(line, paused)) {
            if (paused) {
                returnToLobby = true;
                return true;
            }
            continue;
        }
        if (line.empty()) {
            continue;
        }
        int discard_idx;
        if (!tryParseInt(line, discard_idx) || discard_idx < 0 || discard_idx >= static_cast<int>(hand.size())) {
            std::cout << "Invalid Input!\n";
            sleepMs(1000);
            continue;
        }
        Card* c = hand[discard_idx];
        if (c == nullptr) {
            hand.erase(hand.begin() + discard_idx);
            continue;
        }
        deck.discardCard(c);
        hand.erase(hand.begin() + discard_idx);
        std::cout << "Discarded【" << c->getName() << "】\n";
        sleepMs(500);
    }
    return false;
}
void Player::resetRoundEffects() {
    nextStrikeBonusDamage = 0;
    nextStrikeIgnoreDodge = false;
}

bool Player::hasCard(CardType type) const {
    for (const auto& card : hand) {
        if (card != nullptr && card->type == type) return true;
    }
    return false;
}

bool Player::removeCard(CardType type) {
    for (size_t i = 0; i < hand.size(); ++i) {
        if (hand[i] != nullptr && hand[i]->type == type) {
            deck.discardCard(hand[i]);
            hand.erase(hand.begin() + i);
            return true;
        }
    }
    return false;
}

bool Player::respondToAttack(int requiredShan) {
    int shanCount = 0;
    std::vector<size_t> shanIndices;
    for (size_t i = 0; i < hand.size(); ++i) {
        if (hand[i] != nullptr && hand[i]->type == CardType::SHAN) {
            shanIndices.push_back(i);
            shanCount++;
            if (shanCount >= requiredShan) break;
        }
    }

    int shaCount = 0;
    std::vector<size_t> shaIndices;
    if (hasSkill("Dragon Gut")) {
        for (size_t i = 0; i < hand.size(); ++i) {
            if (hand[i] != nullptr && hand[i]->type == CardType::SHA) {
                shaIndices.push_back(i);
                shaCount++;
                if (shanCount + shaCount >= requiredShan) break;
            }
        }
    }

    if (shanCount >= requiredShan || (hasSkill("Dragon Gut") && shanCount + shaCount >= requiredShan)) {
        int choice = -1;
        std::string prompt = "You have " + std::to_string(shanCount) + " Dodge card(s)";
        if (hasSkill("Dragon Gut")) {
            prompt += " and " + std::to_string(shaCount) + " Strike card(s) usable as Dodge";
        }
        prompt += ". Do you want to use " + std::to_string(requiredShan) + " to dodge? (1=yes, 0=no): ";
        bool first = true;
        while (choice != 0 && choice != 1) {
            if (first) {
                std::cout << "\n" << prompt;
                first = false;
            }
            int key = getKey();
            if (key == '1') {
                choice = 1;
                std::cout << "1\n";
            } else if (key == '0') {
                choice = 0;
                std::cout << "0\n";
            } else {
                std::cout << "\rInvalid input. Please enter 1 to dodge or 0 to take damage.";
                sleepMs(1000);
                std::cout << "\r" << prompt;
            }
        }

        if (choice == 1) {
            int remaining = requiredShan;
            int usedShan = std::min(shanCount, remaining);
            int usedSha = 0;
            if (usedShan < remaining && hasSkill("Dragon Gut")) {
                usedSha = remaining - usedShan;
            }
            std::cout << "\nYou used ";
            if (usedShan > 0) {
                std::cout << usedShan << " Dodge";
                if (usedSha > 0) std::cout << " and ";
            }
            if (usedSha > 0) {
                std::cout << usedSha << " Strike(s) as Dodge";
            }
            std::cout << " to avoid damage from enemy！\n";
            sleepMs(1000);

            for (int i = usedShan - 1; i >= 0; --i) {
                size_t idx = shanIndices[i];
                deck.discardCard(hand[idx]);
                hand.erase(hand.begin() + idx);
            }
            for (int i = usedSha - 1; i >= 0; --i) {
                size_t idx = shaIndices[i];
                deck.discardCard(hand[idx]);
                hand.erase(hand.begin() + idx);
            }
            return true;
        }

        std::cout << "\nYou chose not to dodge and take the damage.\n";
        sleepMs(1000);
    }
    return false;
}

