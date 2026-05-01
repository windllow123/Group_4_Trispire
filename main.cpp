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
            std::cout << "你战败了！游戏结束\n";
            sleepMs(1000);
            break;
        }
        if (enemy.hp <= 0) {
            std::cout << "🎉 击败敌人！获得技能：\n";
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
        std::cout << "\n=== 回合开始 ===";
        sleepMs(1000);
        player.resetShaCount();
        std::cout << "\n抽2张牌！\n";
        sleepMs(1000);
        player.drawTwo();

        // 出牌阶段（可多次出牌）
        while (true) {
            player.showHand();
            std::cout << "\n选择操作：\n0-9 出牌 | -1 结束回合";
            if (player.hasSkill("苦肉")) std::cout << " | -2 使用苦肉";
            std::cout << "\n请输入：";
            int idx;
            std::cin >> idx;

            if (idx == -1) break; // 结束回合
            if (idx == -2) {
                player.useSkillKuRou();
                continue;
            }
            if (idx < 0 || idx >= static_cast<int>(player.hand.size())) {
                std::cout << "输入无效！\n";
                continue;
            }

            Card c = player.hand[idx];
            // 出杀限制
            if (c.type == CardType::SHA && player.current_sha_used >= player.max_sha_per_turn) {
                std::cout << "本回合出杀次数已用完！\n";
                continue;
            }
            // 闪卡限制：没有龙胆技能不能出闪
            if (c.type == CardType::SHAN && !player.hasSkill("龙胆")) {
                std::cout << "没有龙胆技能，无法在回合中出闪！\n";
                continue;
            }
            // 龙胆下闪当杀，也受杀的限制
            if (c.type == CardType::SHAN && player.hasSkill("龙胆") && 
                player.current_sha_used >= player.max_sha_per_turn) {
                std::cout << "本回合出杀次数已用完！\n";
                continue;
            }

            // 打出卡牌
            player.deck.discardCard(c);
            player.hand.erase(player.hand.begin() + idx);

            // 卡牌效果
            if (c.type == CardType::SHA) {
                if (player.hasSkill("龙胆")) {
                    // 杀当闪
                    std::cout << "使用【杀】（龙胆）进行攻击！\n";
                    sleepMs(1000);
                    if (!enemy.respondToAttack()) {
                        enemy.takeDamage(1);
                    }
                } else {
                    // 正常杀
                    player.current_sha_used++;
                    std::cout << "\n你对敌人使用【杀】！\n";
                    sleepMs(1000);
                    bool forceHit = false;
                    if (player.hasSkill("铁骑")) {
                        if (rand() % 2 == 0) {
                            forceHit = true;
                            std::cout << "铁骑发动！杀强制命中！\n";
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
                std::cout << "\n你对敌人使用【闪】（龙胆）！\n";
                sleepMs(1000);
                if (!enemy.respondToAttack()) {
                    enemy.takeDamage(1);
                }
            } else if (c.type == CardType::TAO) {
                player.hp += 1;
                if (player.hp > player.max_hp) player.hp = player.max_hp;
                std::cout << "使用【桃】，恢复1点血量！\n";
                sleepMs(1000);
            }

            if (enemy.hp <= 0) {
                break;
            }
            
            clearScreen();
            displayHealthBar(player, enemy);
        }

        if (enemy.hp <= 0) {
            std::cout << "🎉 击败敌人！获得技能：\n";
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
            std::cout << "\n=== 敌人回合 ===";
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
