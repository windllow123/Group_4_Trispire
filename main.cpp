#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#include "BattleUI.h"
#include "Card.h"
#include "ConsoleUtils.h"
#include "Enemy.h"
#include "Level.h"
#include "Player.h"
#include "SaveLoad.h"
#include "Skill.h"
#include "menu.h"
#include "HowToPlay.h"

namespace {
struct ExitCheckpoint {
    int slot = -1;
    int difficulty = 0;
    int level = 0;
    int hp = 0;
    int maxHp = 0;
    std::vector<Skill> skills;
};
ExitCheckpoint g_exitCheckpoint;
} // namespace

enum class PauseResult { Resume, Lobby };

PauseResult returnToLobbyFromPause(int saveSlot, int difficulty, int currentLevel, int checkpointHp,
                                    int checkpointMaxHp, const std::vector<Skill>& checkpointSkills) {
    clearScreen();
    std::cout << "Game Paused\n";
    std::cout << "Press R to resume, S to write checkpoint to slot " << saveSlot
              << ", Esc or Space to return to the lobby...\n";
    std::cout << "Checkpoint is the start-of-level state (HP/skills/level/difficulty).\n";
    while (true) {
        int key = getKey();
        if (key == 'r' || key == 'R') {
            return PauseResult::Resume;
        }
        if (key == 's' || key == 'S') {
            SaveLoad::saveGame(saveSlot, difficulty, currentLevel, checkpointHp, checkpointMaxHp, checkpointSkills);
        }
        if (key == esc_key_idx || key == space_key_idx) {
            return PauseResult::Lobby;
        }
    }
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    while (true) {
        int choice = showMainMenu();
        int difficulty = 0;
        int startLevel = 1;
        int activeSaveSlot = -1;
        Player player;

        switch (choice) {
            case 1: {
                difficulty = showDifficultyMenu();
                if (difficulty == -1) {
                    continue;
                }
                activeSaveSlot = showSaveSlotMenu(true);
                if (activeSaveSlot == -1) {
                    continue;
                }
                clearScreen();
                std::cout << "New game will be saved to slot " << activeSaveSlot << ".\n";
                pauseConsole();
                break;
            }
            case 2: {
                activeSaveSlot = showSaveSlotMenu(false);
                if (activeSaveSlot == -1) {
                    continue;
                }
                SaveSlotInfo slotInfo;
                if (!SaveLoad::peekSlot(activeSaveSlot, slotInfo)) {
                    clearScreen();
                    std::cout << "No save data in slot " << activeSaveSlot << ".\n";
                    pauseConsole();
                    continue;
                }
                if (!SaveLoad::loadGame(activeSaveSlot, difficulty, startLevel, player)) {
                    clearScreen();
                    std::cout << "Failed to load slot " << activeSaveSlot << ".\n";
                    pauseConsole();
                    continue;
                }
                clearScreen();
                std::cout << "Loaded slot " << activeSaveSlot << ". Resuming from level " << startLevel
                          << " (battle starts fresh at level entry).\n";
                pauseConsole();
                break;
            }
            case 3: {
                std::cout << "How to Play:\n";
                HowToPlay();
                continue;
            }
            case 4: {
                std::cout << "Archive feature not implemented yet.\n";
                pauseConsole();
                continue;
            }
            case 5:
                if (g_exitCheckpoint.slot >= 1) {
                    SaveLoad::saveGame(g_exitCheckpoint.slot, g_exitCheckpoint.difficulty, g_exitCheckpoint.level,
                                       g_exitCheckpoint.hp, g_exitCheckpoint.maxHp, g_exitCheckpoint.skills);
                }
                return 0;
            default: {
                std::cout << "Invalid choice.\n";
                pauseConsole();
                continue;
            }
        }

        if (choice == 1 || choice == 2) {
            g_exitCheckpoint = ExitCheckpoint{};
            if (choice == 1) {
                player.applyDifficulty(difficulty);
            }

            bool returnToLobby = false;
            int checkpointHp = player.hp;
            int checkpointMaxHp = player.max_hp;

            const int lastLevel = SaveLoad::maxLevelForDifficulty(difficulty);
            for (int currentLevel = startLevel; currentLevel <= lastLevel && !returnToLobby; currentLevel++) {
                player.resetDeckAndHandForLevelEntry();
                checkpointHp = player.hp;
                checkpointMaxHp = player.max_hp;
                SaveLoad::saveGame(activeSaveSlot, difficulty, currentLevel, checkpointHp, checkpointMaxHp,
                                   player.skills);
                g_exitCheckpoint.slot = activeSaveSlot;
                g_exitCheckpoint.difficulty = difficulty;
                g_exitCheckpoint.level = currentLevel;
                g_exitCheckpoint.hp = checkpointHp;
                g_exitCheckpoint.maxHp = checkpointMaxHp;
                g_exitCheckpoint.skills = player.skills;

                Level level(currentLevel);
                Enemy enemy;
                level.initializeEnemy(enemy, difficulty);

                BattleUI ui;
                int round = 1;
                ui.addLog("Battle started: Level " + std::to_string(currentLevel) + " vs " + enemy.name + ".");
                ui.addLog("Use Left/Right + Enter to play cards.");

                while (player.hp > 0 && enemy.hp > 0 && !returnToLobby) {
                    player.resetShaCount();
                    player.resetRoundEffects();
                    player.drawTwo();
                    ui.addLog("Round " + std::to_string(round) + " started. Drew 2 cards.");

                    int selectedCard = 0;
                    std::string helper = "Choose a card, press Enter to play, or E to end turn.";
                    bool endPlayerTurn = false;

                    auto clampSelected = [&]() {
                        if (player.hand.empty()) {
                            selectedCard = 0;
                            return;
                        }
                        int visibleCards = std::min(8, static_cast<int>(player.hand.size()));
                        selectedCard = std::max(0, std::min(selectedCard, visibleCards - 1));
                    };

                    while (!endPlayerTurn && enemy.hp > 0 && player.hp > 0 && !returnToLobby) {
                        clampSelected();
                        ui.render(player, enemy, currentLevel, round, selectedCard, "Your Turn", helper);

                        int key = getKey();

                        if (isPauseKey(key)) {
                            PauseResult pauseResult =
                                returnToLobbyFromPause(activeSaveSlot, difficulty, currentLevel, checkpointHp,
                                                       checkpointMaxHp, player.skills);
                            if (pauseResult == PauseResult::Lobby) {
                                returnToLobby = true;
                                break;
                            }
                            helper = "Resumed.";
                            continue;
                        }

                        if (key == left_key_idx && !player.hand.empty()) {
                            int visibleCards = std::min(8, static_cast<int>(player.hand.size()));
                            selectedCard = (selectedCard + visibleCards - 1) % visibleCards;
                            helper = "Card selection moved left.";
                            continue;
                        }
                        if (key == right_key_idx && !player.hand.empty()) {
                            int visibleCards = std::min(8, static_cast<int>(player.hand.size()));
                            selectedCard = (selectedCard + 1) % visibleCards;
                            helper = "Card selection moved right.";
                            continue;
                        }
                        if (key == up_key_idx && !player.hand.empty()) {
                            if (selectedCard >= 4) {
                                selectedCard -= 4;
                                helper = "Card selection moved up.";
                            } else {
                                helper = "Already at the top row.";
                            }
                            continue;
                        }
                        if (key == down_key_idx && !player.hand.empty()) {
                            int visibleCards = std::min(8, static_cast<int>(player.hand.size()));
                            if (selectedCard < 4 && selectedCard + 4 < visibleCards) {
                                selectedCard += 4;
                                helper = "Card selection moved down.";
                            } else {
                                helper = "Already at the bottom row.";
                            }
                            continue;
                        }

                        if (key == 'e' || key == 'E') {
                            ui.addLog("You ended your turn.");
                            endPlayerTurn = true;
                            continue;
                        }

                        if (key == 'q' || key == 'Q') {
                            if (!player.hasSkill("Sacrifice")) {
                                helper = "You do not have Sacrifice.";
                            } else {
                                int hpBefore = player.hp;
                                size_t handBefore = player.hand.size();
                                bool ok = player.useSkillKuRou();
                                if (ok) {
                                    ui.addLog("Sacrifice used: -" + std::to_string(hpBefore - player.hp) + " HP, +" +
                                              std::to_string(static_cast<int>(player.hand.size() - handBefore)) + " cards.");
                                    helper = "Sacrifice succeeded.";
                                } else {
                                    helper = "Sacrifice failed (likely not enough HP).";
                                }
                            }
                            continue;
                        }

                        if (key != enter_key_idx) {
                            helper = "Invalid key. Use Left/Right + Enter, E, Q.";
                            continue;
                        }

                        if (player.hand.empty()) {
                            helper = "No cards to play.";
                            continue;
                        }

                        int playIndex = selectedCard;
                        if (playIndex < 0 || playIndex >= static_cast<int>(player.hand.size())) {
                            helper = "That card slot is empty.";
                            continue;
                        }

                        Card* c = player.hand[playIndex];
                        if (c == nullptr) {
                            helper = "Invalid card slot.";
                            continue;
                        }

                        if (c->type == CardType::SHA && player.current_sha_used >= player.max_sha_per_turn) {
                            helper = "No Strike actions left this round.";
                            continue;
                        }
                        if (c->type == CardType::SHAN && !player.hasSkill("Dragon Gut")) {
                            helper = "Dodge can only attack with Dragon Gut.";
                            continue;
                        }
                        if (c->type == CardType::SHAN && player.hasSkill("Dragon Gut") &&
                            player.current_sha_used >= player.max_sha_per_turn) {
                            helper = "No Strike actions left this round.";
                            continue;
                        }

                        player.deck.discardCard(c);
                        player.hand.erase(player.hand.begin() + playIndex);

                        if (c->type == CardType::SHA) {
                            player.current_sha_used++;

                            int damage = 1;
                            bool ignoreDodge = false;
                            if (player.nextStrikeBonusDamage > 0) {
                                damage += player.nextStrikeBonusDamage;
                                ignoreDodge = player.nextStrikeIgnoreDodge;
                                player.resetRoundEffects();
                            }

                            bool forceHit = player.hasSkill("Steel Cavalry");
                            if (forceHit) {
                                ui.addLog("Steel Cavalry: Strike cannot be dodged.");
                            }

                            int enemyHpBefore = enemy.hp;
                            bool dodged = (!forceHit && !ignoreDodge && enemy.respondToAttack());
                            if (dodged) {
                                ui.addLog("You played Strike, but enemy dodged.");
                            } else {
                                enemy.takeDamage(damage);
                                ui.addLog("You played Strike and dealt " + std::to_string(enemyHpBefore - enemy.hp) + " damage.");
                            }
                            helper = "Striked.";
                        } else if (c->type == CardType::SHAN) {
                            player.current_sha_used++;

                            int damage = 1;
                            bool ignoreDodge = false;
                            if (player.nextStrikeBonusDamage > 0) {
                                damage += player.nextStrikeBonusDamage;
                                ignoreDodge = player.nextStrikeIgnoreDodge;
                                player.resetRoundEffects();
                            }

                            bool forceHitDg = player.hasSkill("Steel Cavalry");
                            if (forceHitDg) {
                                ui.addLog("Steel Cavalry: attack cannot be dodged.");
                            }
                            int enemyHpBefore = enemy.hp;
                            bool dodged = (!forceHitDg && !ignoreDodge && enemy.respondToAttack());
                            if (dodged) {
                                ui.addLog("Dragon Gut attack with Dodge was dodged by enemy.");
                            } else {
                                enemy.takeDamage(damage);
                                ui.addLog("Dragon Gut attack with Dodge dealt " + std::to_string(enemyHpBefore - enemy.hp) + " damage.");
                            }
                            helper = "Dragon Strike!";
                        } else if (c->type == CardType::TAO) {
                            int oldHp = player.hp;
                            player.hp = std::min(player.max_hp, player.hp + 1);
                            if (player.hp == oldHp) {
                                ui.addLog("You played Heal, but it has no effect because you are already at max health.");
                            } else {
                                ui.addLog("You played Heal and recovered " + std::to_string(player.hp - oldHp) + " HP.");
                            }
                            helper = "Heal succeeded.";
                        } else if (c->type == CardType::TOTEM) {
                            player.nextStrikeBonusDamage = 1;
                            player.nextStrikeIgnoreDodge = true;
                            ui.addLog("You used The Totem: next strike this round is +1 damage and cannot be dodged.");
                            helper = "Totem buff is active.";
                        }
                    }

                    if (returnToLobby || enemy.hp <= 0) {
                        break;
                    }

                    bool discardReturnToLobby = false;
                    if (player.discardExcessCards(enemy, discardReturnToLobby)) {
                        if (discardReturnToLobby) {
                            returnToLobby = true;
                            break;
                        }
                    }
                    ui.addLog("End of player turn: discard phase completed.");

                    if (enemy.hp > 0 && !returnToLobby) {
                        ui.render(player, enemy, currentLevel, round, selectedCard, "Enemy Turn", "Enemy is acting...");
                        sleepMs(500);

                        int hpBeforeEnemyTurn = player.hp;
                        int enemyHpBeforeRegen = enemy.hp;
                        size_t enemyHandBefore = enemy.hand.size();

                        if (round % 5 == 0) {
                            enemy.regenerate();
                            if (enemy.hp > enemyHpBeforeRegen) {
                                ui.addLog("Enemy regenerated " + std::to_string(enemy.hp - enemyHpBeforeRegen) + " HP.");
                            }
                        }

                        enemy.drawOne();
                        ui.addLog("Enemy drew 1 card.");

                        enemy.playCards();
                        bool enemyActuallyAttacked = enemy.attack(player, ui);

                        if (enemyActuallyAttacked) {
                            if (player.hp < hpBeforeEnemyTurn) {
                                ui.addLog("Enemy attack dealt " + std::to_string(hpBeforeEnemyTurn - player.hp) + " damage to you.");
                            } else {
                                ui.addLog("Enemy attack dealt no damage.");
                            }
                        } else {
                            ui.addLog("Enemy didn't attack.");
                        }

                        enemy.discardExcessCards();
                        if (enemy.hand.size() < enemyHandBefore) {
                            ui.addLog("Enemy discarded down to hand limit.");
                        }
                    }

                    round++;
                }

                if (returnToLobby) {
                    break;
                }

                if (player.hp <= 0) {
                    clearScreen();
                    std::cout << "You Lose! Game Over!\n";
                    std::cout << "For a lost game, the slot's record will remain at the level they lost the game.\n";
                    pauseConsole();
                    break;
                }

                std::cout << "Defeated " << enemy.name << "! Claimed Skill:\n";
                sleepMs(500);
                Skill s = Skill::getRandomSkill();
                while (player.hasSkill(s.name)) {
                    s = Skill::getRandomSkill();
                }
                player.addSkill(s);
                pauseConsole();
            }

            if (returnToLobby) {
                continue;
            }

            if (player.hp > 0) {
                clearScreen();
                std::cout << "Game Successful! You have completed all levels!\n";
                std::cout << "For a game already played fully and successful, the slot's record will be emptied.\n";
                SaveLoad::clearSaveSlot(activeSaveSlot);
                g_exitCheckpoint = ExitCheckpoint{};
                pauseConsole();
            }
        }
    }

    return 0;
}
