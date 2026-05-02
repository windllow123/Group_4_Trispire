#include "Player.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <sstream>
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

bool Player::useSkillKuRou() {
    if (hp > 1 && hasSkill("Sacrifice")) {
        hp -= 1;
        std::cout << "Using【Sacrifice】，Lost 1 point of health，Aquire 2 cards!\n";
        sleepMs(1000);
        drawTwo();
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
        std::cout << "Enter card index: ";
        std::string line;
        std::getline(std::cin, line);
        if (line.empty()) {
            std::getline(std::cin, line);
        }
        int discard_idx;
        if (!tryParseInt(line, discard_idx) || discard_idx < 0 || discard_idx >= static_cast<int>(hand.size())) {
            std::cout << "Invalid Input!\n";
            continue;
        }
        Card c = hand[discard_idx];
        deck.discardCard(c);
        hand.erase(hand.begin() + discard_idx);
        std::cout << "Discarded【" << c.getName() << "】\n";
        sleepMs(500);
    }
}

void Player::resetRoundEffects() {
    nextStrikeBonusDamage = 0;
    nextStrikeIgnoreDodge = false;
}

bool Player::hasCard(CardType type) const {
    for (const auto& card : hand) {
        if (card.type == type) return true;
    }
    return false;
}

bool Player::removeCard(CardType type) {
    for (size_t i = 0; i < hand.size(); ++i) {
        if (hand[i].type == type) {
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
        if (hand[i].type == CardType::SHAN) {
            shanIndices.push_back(i);
            shanCount++;
            if (shanCount >= requiredShan) break;
        }
    }

    int shaCount = 0;
    std::vector<size_t> shaIndices;
    if (hasSkill("Dragon Gut")) {
        for (size_t i = 0; i < hand.size(); ++i) {
            if (hand[i].type == CardType::SHA) {
                shaIndices.push_back(i);
                shaCount++;
                if (shanCount + shaCount >= requiredShan) break;
            }
        }
    }

    if (shanCount >= requiredShan || (hasSkill("Dragon Gut") && shanCount + shaCount >= requiredShan)) {
        int choice = -1;
        while (choice != 0 && choice != 1) {
            std::cout << "\nYou have " << shanCount << " Dodge card(s)";
            if (hasSkill("Dragon Gut")) {
                std::cout << " and " << shaCount << " Strike card(s) usable as Dodge";
            }
            std::cout << ". Do you want to use " << requiredShan << " to dodge? (1=yes, 0=no): ";
            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                choice = -1;
                std::cout << "Invalid input. Please enter 1 to dodge or 0 to take damage.\n";
                continue;
            }
            if (choice != 0 && choice != 1) {
                std::cout << "Invalid input. Please enter 1 to dodge or 0 to take damage.\n";
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
