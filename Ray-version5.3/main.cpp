#include <iostream>
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
                std::cout << "Strike deals damage, Dodge avoids damage, Heal restores HP.\n";
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
            }

            for (int currentLevel = startLevel; currentLevel <= 5; currentLevel++) {
                Level level(currentLevel);
                Enemy enemy;
                level.initializeEnemy(enemy, difficulty);

                std::cout << "\n=== Level " << currentLevel << ": " << enemy.name << " ===\n";
                pauseConsole();

                // 每场战斗标记
                bool isFirstRound = true;      // 用于闪电战
                bool hasUsedDesperation = false; // 本场战斗是否用过绝境（也可以用player.desperation_used_in_battle，这里独立）
                // 战斗开始前重置玩家的一些战斗相关状态
                player.tenacity_damage_counter = 0;
                player.blitz_used_in_battle = false;
                player.blitz_extra_sha = 0;
                player.desperation_used_in_battle = false;

                // 战斗循环
                while (player.hp > 0 && enemy.hp > 0) {
                    clearScreen();
                    std::cout << "Level " << currentLevel << " - " << enemy.name << "\n";
                    displayHealthBar(player, enemy);
                    player.showStatus();
                    enemy.showStatus();
                    sleepMs(1000);

                    // 玩家回合开始
                    std::cout << "\n=== Round Start! Fight! ===";
                    sleepMs(1000);
                    player.resetShaCount();
                    
                    // 闪电战：第一回合且未使用过
                    // 正常抽2张牌
                    std::cout << "\n2 Cards drawn！\n";
                    sleepMs(1000);
                    player.drawTwo();

                    // 闪电战：第一回合且技能存在且本场未使用
                    if (isFirstRound && player.hasSkill("Blitz") && !player.blitz_used_in_battle) {
                        player.drawTwo();  // 额外再抽2张
                        player.max_sha_per_turn++;
                        player.blitz_extra_sha = 1;
                        player.blitz_used_in_battle = true;
                        std::cout << "【Blitz】Triggered! Draw 2 extra cards, +1 Strike this round.\n";
                        sleepMs(1000);
                        isFirstRound = false;
                    }

                    // 出牌阶段
                    while (true) {
                        player.showHand();
                        std::cout << "\nAct:\n0-9 use card | -1 Round End";
                        if (player.hasSkill("Sacrifice")) std::cout << " | -2 used 【Sacrifice】";
                        std::cout << "\nPlease enter：";
                        int idx;
                        std::cin >> idx;

                        if (idx == -1) break;
                        if (idx == -2) {
                            player.useSkillKuRou();
                            continue;
                        }
                        if (idx < 0 || idx >= static_cast<int>(player.hand.size())) {
                            std::cout << "Invalid Input！\n";
                            continue;
                        }

                        Card c = player.hand[idx];
                        if (c.type == CardType::SHA && player.current_sha_used >= player.max_sha_per_turn) {
                            std::cout << "You have spent all【strike】chances for this round!\n";
                            continue;
                        }
                        if (c.type == CardType::SHAN && !player.hasSkill("Dragon Gut")) {
                            std::cout << "Cannot use Dodge in your own round!\n";
                            continue;
                        }
                        if (c.type == CardType::SHAN && player.hasSkill("Dragon Gut") &&
                            player.current_sha_used >= player.max_sha_per_turn) {
                            std::cout << "You have spent all【strike】chances for this round!\n";
                            continue;
                        }

                        clearScreen();
                        displayHealthBar(player, enemy);

                        // 打出卡牌
                        player.deck.discardCard(c);
                        player.hand.erase(player.hand.begin() + idx);

                        // 处理卡牌效果
                        if (c.type == CardType::SHA) {
                            if (player.hasSkill("Dragon Gut")) {
                                // 龙胆下杀当闪？这里逻辑是杀当闪，但attack？暂按原逻辑攻击
                                std::cout << "Using【Strike】to attack!\n";
                                sleepMs(1000);
                                bool enemyDodged = enemy.respondToAttack();
                                if (!enemyDodged) {
                                    enemy.takeDamage(1);
                                    // 吸血：造成的伤害来自杀
                                    if (player.hasSkill("Vampire") && player.vampire_used_this_turn == 0) {
                                        player.hp = std::min(player.hp + 1, player.max_hp);
                                        std::cout << "【Vampire】Healed 1 HP!\n";
                                        player.vampire_used_this_turn = 1;
                                        sleepMs(500);
                                    }
                                }
                            } else {
                                player.current_sha_used++;
                                std::cout << "\nUsing【Strike】to attack!\n";
                                sleepMs(1000);
                                bool forceHit = false;
                                if (player.hasSkill("Steel Cavalry")) {
                                    if (rand() % 2 == 0) {
                                        forceHit = true;
                                        std::cout << "【Steel Cavalry】Triggered! Force hit causing damage!\n";
                                        sleepMs(1000);
                                    }
                                }
                                bool enemyDodged = false;
                                if (!forceHit) {
                                    enemyDodged = enemy.respondToAttack();
                                }
                                if (!enemyDodged) {
                                    enemy.takeDamage(1);
                                    // 吸血
                                    if (player.hasSkill("Vampire") && player.vampire_used_this_turn == 0) {
                                        player.hp = std::min(player.hp + 1, player.max_hp);
                                        std::cout << "【Vampire】Healed 1 HP!\n";
                                        player.vampire_used_this_turn = 1;
                                        sleepMs(500);
                                    }
                                }
                            }
                        } else if (c.type == CardType::SHAN) {
                            // 龙胆下闪当杀
                            player.current_sha_used++;
                            std::cout << "\nUsing【Dodge】to attack!【Dragon Gut】！\n";
                            sleepMs(1000);
                            bool enemyDodged = enemy.respondToAttack();
                            if (!enemyDodged) {
                                enemy.takeDamage(1);
                                // 吸血：用闪当杀造成的伤害
                                if (player.hasSkill("Vampire") && player.vampire_used_this_turn == 0) {
                                    player.hp = std::min(player.hp + 1, player.max_hp);
                                    std::cout << "【Vampire】Healed 1 HP!\n";
                                    player.vampire_used_this_turn = 1;
                                    sleepMs(500);
                                }
                            }
                        } else if (c.type == CardType::TAO) {
                            player.hp = std::min(player.hp + 1, player.max_hp);
                            std::cout << "Using【Heal】, regenerates 1 hp!\n";
                            if (player.hasSkill("Rally")) {
                                player.drawOne();
                                std::cout << "【Rally】Draw 1 extra card!\n";
                                sleepMs(500);
                            }
                            sleepMs(1000);
                        }

                        // 绝境检查（每次出牌后检查，避免死亡前）
                        if (player.hasSkill("Desperation") && !player.desperation_used_in_battle) {
                            int threshold = (player.max_hp * 3 + 9) / 10; // 向上取整30%，即 (max_hp*3+9)/10
                            if (player.hp <= threshold && player.hand.empty()) {
                                // 将弃牌堆洗回牌堆
                                player.deck.draw_pile.insert(player.deck.draw_pile.end(), 
                                    player.deck.discard_pile.begin(), player.deck.discard_pile.end());
                                player.deck.discard_pile.clear();
                                player.deck.shuffle();
                                // 抽3张
                                for (int i = 0; i < 3; i++) {
                                    player.hand.push_back(player.deck.drawCard());
                                }
                                player.desperation_used_in_battle = true;
                                std::cout << "【Desperation】Triggered! Discard pile shuffled, draw 3 cards!\n";
                                sleepMs(1000);
                            }
                        }

                        if (enemy.hp <= 0) {
                            break;
                        }

                        clearScreen();
                        displayHealthBar(player, enemy);
                    } // 出牌循环结束

                    if (enemy.hp <= 0) {
                        break; // 跳出战斗循环，获得技能
                    }

                    // 弃牌阶段
                    player.discardExcessCards();
                    sleepMs(1000);
                    clearScreen();
                    displayHealthBar(player, enemy);

                    // 敌人回合
                    if (enemy.hp > 0) {
                        std::cout << "\n=== Enemy Round ===";
                        sleepMs(1000);
                        enemy.regenerate();
                        enemy.drawOne();
                        enemy.playCards();
                        player.showHand();
                        
                        // 记录扣血前的血量，用于坚韧和荆棘
                        int hpBefore = player.hp;
                        enemy.attack(player);
                        int hpLost = hpBefore - player.hp;
                        
                        // 荆棘：受到伤害时反伤
                        if (hpLost > 0 && player.hasSkill("Thorns")) {
                            enemy.takeDamage(1);
                            std::cout << "【Thorns】Dealt 1 damage to enemy!\n";
                            sleepMs(500);
                        }
                        // 坚韧：累计失去生命抽牌
                        if (hpLost > 0 && player.hasSkill("Tenacity")) {
                            player.tenacity_damage_counter += hpLost;
                            while (player.tenacity_damage_counter >= 2) {
                                player.tenacity_damage_counter -= 2;
                                player.drawOne();
                                std::cout << "【Tenacity】Lost 2 HP, draw 1 card.\n";
                                sleepMs(500);
                            }
                        }
                        
                        clearScreen();
                        displayHealthBar(player, enemy);
                        enemy.discardExcessCards();
                        sleepMs(1000);
                        clearScreen();
                        displayHealthBar(player, enemy);
                    }

                    // 回合结束恢复闪电战的临时加成
                    if (player.blitz_extra_sha > 0) {
                        player.max_sha_per_turn--;
                        player.blitz_extra_sha = 0;
                    }

                    pauseConsole();
                } // 战斗循环结束

                // 战斗结束，判断胜负
                if (player.hp <= 0) {
                    std::cout << "You Lose! Game Over!\n";
                    SaveLoad::saveGame(difficulty, currentLevel, player);
                    sleepMs(1000);
                    pauseConsole();
                    break;
                }

                // 击败敌人，三选一技能
                std::cout << "Defeated " << enemy.name << "! Choose a skill:\n";
                sleepMs(1000);
                std::vector<Skill> skills = Skill::getRandomSkills(3);
                for (size_t i = 0; i < skills.size(); ++i) {
                    std::cout << i + 1 << ". 【" << skills[i].name << "】" << skills[i].desc << "\n";
                }
                int skillChoice = 0;
                while (skillChoice < 1 || skillChoice > (int)skills.size()) {
                    std::cout << "Enter 1-" << skills.size() << ": ";
                    std::cin >> skillChoice;
                    if (std::cin.fail()) {
                        std::cin.clear();
                        std::cin.ignore(1000, '\n');
                        skillChoice = 0;
                    }
                }
                Skill s = skills[skillChoice - 1];
                player.addSkill(s);
                std::cout << "You acquired 【" << s.name << "】\n";
                sleepMs(1000);
                pauseConsole();
            } // 关卡循环结束

            if (player.hp > 0) {
                std::cout << "Congratulations! You have completed all 5 levels!\n";
                SaveLoad::deleteSaveFile();
                pauseConsole();
            }
        }
    }
    return 0;
}