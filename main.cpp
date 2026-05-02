#include <iostream>
#include <sstream>
#include "ConsoleUtils.h"
#include <cstdlib>
#include <ctime>
#include "Card.h"
#include "Player.h"
#include "Enemy.h"
#include "Skill.h"
#include "menu.h"
#include "Level.h"
#include "SaveLoad.h"

bool tryParseInt(const std::string& input, int& value) {
    std::istringstream iss(input);
    if (!(iss >> value)) return false;
    char extra;
    return !(iss >> extra);
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    while (true) {
        int choice = showMainMenu();
        int difficulty = 0;
        int startLevel = 1;
        Player player;

        switch (choice) {
            case 1: { // New Game
                difficulty = showDifficultyMenu();
                std::cout << "Selected difficulty: " << difficulty << std::endl;
                pauseConsole();
                break;
            }
            case 2: { // Load Game
                if (SaveLoad::loadGame(difficulty, startLevel, player)) {
                    std::cout << "Game loaded successfully. Starting from level " << startLevel << std::endl;
                    pauseConsole();
                } else {
                    std::cout << "No save file found. Starting new game.\n";
                    difficulty = showDifficultyMenu();
                    std::cout << "Selected difficulty: " << difficulty << std::endl;
                    pauseConsole();
                }
                break;
            }
            case 3: { // How to Play
                std::cout << "How to Play:\n";
                std::cout << "This is a card battle game. Use cards to defeat enemies and gain skills.\n";
                std::cout << "Strike deals damage, Dodge avoids damage, Heal restores HP, and The Totem buffs your next strike or saves you from defeat.\n";
                pauseConsole();
                continue;
            }
            case 4: { // Archive
                std::cout << "Archive feature not implemented yet.\n";
                pauseConsole();
                continue;
            }
            case 5: { // Quit
                return 0;
            }
            default: {
                std::cout << "Invalid choice.\n";
                pauseConsole();
                continue;
            }
        }

        // Start game
        if (choice == 1 || choice == 2) {
            if (choice == 1) {
                player.applyDifficulty(difficulty);
            } // For load, player is already loaded

            for (int currentLevel = startLevel; currentLevel <= 5; currentLevel++) {
                Level level(currentLevel);
                Enemy enemy;
                level.initializeEnemy(enemy, difficulty);
                
                clearScreen();

                std::cout << "\n=== Level " << currentLevel << ": " << enemy.name << " ===\n";
                pauseConsole();

                // One initialization per level; rounds preserve HP, hands, and decks.
                while (player.hp > 0 && enemy.hp > 0) {
                    clearScreen();
                    std::cout << "Level " << currentLevel << " - " << enemy.name << "\n";
                    displayHealthBar(player, enemy);
                    //player.showStatus();
                    //enemy.showStatus();
                    sleepMs(1000);

                    std::cout << "\n=== Round Start! Fight! ===";
                    sleepMs(1000);
                    player.resetShaCount();
                    player.resetRoundEffects();
                    std::cout << "\n2 Cards drawn！\n";
                    sleepMs(1000);
                    player.drawTwo();

                    while (true) {
                        player.showHand();
                        std::cout << "\nAct:\n0-9 use card | -1 Round End";
                        if (player.hasSkill("Sacrifice")) std::cout << " | -2 used 【Sacrifice】";
                        std::cout << "\nPlease enter：";
                        int idx;
                        std::string input;
                        std::getline(std::cin, input);
                        if (input.empty()) {
                            std::getline(std::cin, input);
                        }
                        if (!tryParseInt(input, idx)) {
                            std::cout << "Invalid Input！ Please enter a valid integer choice.\n";
                            sleepMs(1000);
                            clearScreen();
                            std::cout << "Level " << currentLevel << " - " << enemy.name << "\n";
                            displayHealthBar(player, enemy);
                            continue;
                        }

                        if (idx == -1) break;
                        if (idx == -2) {
                            if (!player.useSkillKuRou()) {
                                sleepMs(1000);
                                clearScreen();
                                std::cout << "Level " << currentLevel << " - " << enemy.name << "\n";
                                displayHealthBar(player, enemy);
                            }
                            continue;
                        }
                        if (idx < 0 || idx >= static_cast<int>(player.hand.size())) {
                            std::cout << "Invalid Input！ Please enter a valid integer choice.\n";
                            sleepMs(1000);
                            clearScreen();
                            std::cout << "Level " << currentLevel << " - " << enemy.name << "\n";
                            displayHealthBar(player, enemy);
                            continue;
                        }

                        Card c = player.hand[idx];
                        if (c.type == CardType::SHA && player.current_sha_used >= player.max_sha_per_turn) {
                            std::cout << "You have spent all【strike】chances for this round!\n";
                            sleepMs(1000);
                            clearScreen();
                            displayHealthBar(player, enemy);
                            continue;
                        }
                        if (c.type == CardType::SHAN && !player.hasSkill("Dragon Gut")) {
                            std::cout << "Cannot use Dodge in your own round!\n";
                            sleepMs(1000);
                            clearScreen();
                            displayHealthBar(player, enemy);
                            continue;
                        }
                        if (c.type == CardType::SHAN && player.hasSkill("Dragon Gut") &&
                            player.current_sha_used >= player.max_sha_per_turn) {
                            std::cout << "You have spent all【strike】chances for this round!\n";
                            sleepMs(1000);
                            clearScreen();
                            displayHealthBar(player, enemy);
                            continue;
                        }

                        clearScreen();
                        displayHealthBar(player, enemy);

                        player.deck.discardCard(c);
                        player.hand.erase(player.hand.begin() + idx);

                        if (c.type == CardType::SHA) {
                            player.current_sha_used++;
                            std::cout << "\nUsing【Strike】to attack!\n";
                            sleepMs(1000);
                            int damage = 1;
                            bool ignoreDodge = false;
                            if (player.nextStrikeBonusDamage > 0) {
                                damage += player.nextStrikeBonusDamage;
                                ignoreDodge = player.nextStrikeIgnoreDodge;
                                player.resetRoundEffects();
                            }
                            bool forceHit = false;
                            if (player.hasSkill("Steel Cavalry")) {
                                if (rand() % 2 == 0) {
                                    forceHit = true;
                                    std::cout << "【Steel Cavalry】Triggered! Force hit causing damage!\n";
                                    sleepMs(1000);
                                }
                            }
                            if (!forceHit && !ignoreDodge && enemy.respondToAttack()) {
                                // enemy dodged
                            } else {
                                enemy.takeDamage(damage);
                            }
                        } else if (c.type == CardType::SHAN) {
                            player.current_sha_used++;
                            std::cout << "\nUsing【Dodge】to attack!【Dragon Gut】！\n";
                            sleepMs(1000);
                            int damage = 1;
                            bool ignoreDodge = false;
                            if (player.nextStrikeBonusDamage > 0) {
                                damage += player.nextStrikeBonusDamage;
                                ignoreDodge = player.nextStrikeIgnoreDodge;
                                player.resetRoundEffects();
                            }
                            if (!ignoreDodge && enemy.respondToAttack()) {
                                // enemy dodged
                            } else {
                                enemy.takeDamage(damage);
                            }
                        } else if (c.type == CardType::TAO) {
                            player.hp += 1;
                            if (player.hp > player.max_hp) player.hp = player.max_hp;
                            std::cout << "Using【Heal】, regenerates 1 hp!\n";
                            sleepMs(1000);
                        } else if (c.type == CardType::TOTEM) {
                            player.nextStrikeBonusDamage = 1;
                            player.nextStrikeIgnoreDodge = true;
                            std::cout << "Using【The Totem】! Your next strike this round will deal +1 damage and cannot be dodged by the enemy.\n";
                            sleepMs(2000);
                        }

                        if (enemy.hp <= 0) {
                            clearScreen();
                            displayHealthBar(player, enemy);
                            break;
                        }

                        clearScreen();
                        displayHealthBar(player, enemy);
                    }

                    if (enemy.hp <= 0) {
                        break;
                    }

                    player.discardExcessCards();
                    //sleepMs(1000);
                    pauseConsole();
                    clearScreen();
                    displayHealthBar(player, enemy);

                    if (enemy.hp > 0) {
                        std::cout << "\n=== Enemy Round ===";
                        sleepMs(1000);
                        enemy.regenerate();
                        enemy.drawOne();
                        enemy.playCards();
                        player.showHand();
                        enemy.attack(player);
                        sleepMs(1000);
                        //clearScreen();
                        //displayHealthBar(player, enemy);
                        enemy.discardExcessCards();
                        pauseConsole2();
                        //sleepMs(2000);
                        clearScreen();
                        //displayHealthBar(player, enemy);
                    }

                    //pauseConsole2();
                }

                if (player.hp <= 0) {
                    std::cout << "You Lose! Game Over!\n";
                    SaveLoad::saveGame(difficulty, currentLevel, player);
                    sleepMs(1000);
                    pauseConsole();
                    break;
                }

                std::cout << "Defeated " << enemy.name << "! Claimed Skill：\n";
                sleepMs(1000);
                
                
                Skill s = Skill::getRandomSkill();
                while (player.hasSkill(s.name)) {
                    s = Skill::getRandomSkill();
                }
                
                //Skill s = {5, "Dragon Gut", "You can use【Dodge】as【Strike】，use【Strike】as【Dodge】."};
                player.addSkill(s);
                //std::cout << "【" << s.name << "】" << s.desc << "\n";
                //sleepMs(1000);
                pauseConsole();
            }

            if (player.hp > 0) {
                std::cout << "Congratulations! You have completed all 5 levels!\n";
                SaveLoad::deleteSaveFile();
                pauseConsole();
            }
        }
    }
    return 0;
}
