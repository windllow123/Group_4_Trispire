#include <iostream>
#include "ConsoleUtils.h"
#include <cstdlib>
#include <ctime>
#include "Card.h"
#include "Player.h"
#include "Enemy.h"
#include "Skill.h"
#include "menu.h"


int main() {
    //change main menu
    showMainMenu();
    //parse input
    int difficulty;
    difficulty=showDifficultyMenu();
    std::cout<<"Selected difficulty: "<<difficulty<<std::endl;
    pauseConsole();

    srand(static_cast<unsigned>(time(nullptr)));
    Player player;
    Enemy enemy;
    player.applyDifficulty(difficulty);
    enemy.applyDifficulty(difficulty);

    while (true) {
        clearScreen();
        displayHealthBar(player, enemy);
        // 死亡判断
        if (player.hp <= 0) {
            std::cout << "You Lose! Game Over!\n";
            sleepMs(1000);
            break;
        }
        if (enemy.hp <= 0) {
            std::cout << "Defeated Enemy! Claimed Skill：\n";
            sleepMs(1000);
            Skill s = Skill::getRandomSkill();
            player.addSkill(s);
            std::cout << "【" << s.name << "】" << s.desc << "\n";
            sleepMs(1000);
            enemy = Enemy();
            pauseConsole();
            continue;
        }

        // 状态展示
        player.showStatus();
        enemy.showStatus();
        sleepMs(1000);

        // 回合开始：重置杀次数+抽2张牌
        std::cout << "\n=== Round Start! Fight! ===";
        sleepMs(1000);
        player.resetShaCount();
        std::cout << "\nDrew 2 Cards！\n";
        sleepMs(1000);
        player.drawTwo();

        // 出牌阶段（可多次出牌）
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
            // 出杀限制
            if (c.type == CardType::SHA && player.current_sha_used >= player.max_sha_per_turn) {
                std::cout << "You have spent all【strike】chances for this round!\n";
                continue;
            }
            // 闪卡限制：没有龙胆技能不能出闪
            if (c.type == CardType::SHAN && !player.hasSkill("Dragon Gut")) {
                std::cout << "You do not acquire skill【Dragon Gut】, can't use【Dodge】in your own round!\n";
                continue;
            }
            // 龙胆下闪当杀，也受杀的限制
            if (c.type == CardType::SHAN && player.hasSkill("Dragon Gut") && 
                player.current_sha_used >= player.max_sha_per_turn) {
                std::cout << "You have spent all【strike】chances for this round!\n";
                continue;
            }

            // 打出卡牌
            player.deck.discardCard(c);
            player.hand.erase(player.hand.begin() + idx);

            // 卡牌效果
            if (c.type == CardType::SHA) {
                if (player.hasSkill("Dragon Gut")) {
                    // 杀当闪
                    std::cout << "Using【Strike】to attack!\n";
                    sleepMs(1000);
                    if (!enemy.respondToAttack()) {
                        enemy.takeDamage(1);
                    }
                } else {
                    // 正常杀
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
                    if (!forceHit && enemy.respondToAttack()) {
                        // 敌人成功躲闪
                    } else {
                        enemy.takeDamage(1);
                    }
                }
            } else if (c.type == CardType::SHAN) {
                // 龙胆下闪当杀
                player.current_sha_used++;
                std::cout << "\nUsing【Dodge】to attack!【Dragon Gut】！\n";
                sleepMs(1000);
                if (!enemy.respondToAttack()) {
                    enemy.takeDamage(1);
                }
            } else if (c.type == CardType::TAO) {
                player.hp += 1;
                if (player.hp > player.max_hp) player.hp = player.max_hp;
                std::cout << "Using【Heal】, regenerates 1 hp!\n";
                sleepMs(1000);
            }

            if (enemy.hp <= 0) {
                break;
            }
            
            clearScreen();
            displayHealthBar(player, enemy);
        }

        if (enemy.hp <= 0) {
            std::cout << "Enemy Defeated! Acquires: \n";
            sleepMs(1000);
            Skill s = Skill::getRandomSkill();
            player.addSkill(s);
            std::cout << "【" << s.name << "】" << s.desc << "\n";
            sleepMs(1000);
            enemy = Enemy();
            pauseConsole();
            continue;
        }

        // 出牌阶段结束，弃牌阶段
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
            enemy.attack(player);
            clearScreen();
            displayHealthBar(player, enemy);
            enemy.discardExcessCards();
            sleepMs(1000);
            clearScreen();
            displayHealthBar(player, enemy);
        }

        pauseConsole();
    }
    return 0;
}
