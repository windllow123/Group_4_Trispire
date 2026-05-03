## Group 4 - Trispire

A text-based roguelike deckbuilder inspired by Slay the Spire and Sanguosha, implemented in C++17. The player battles through escalating encounters, acquires skills, and manages a deck of Strike, Dodge, Heal, and Totem cards.

---

## Team Members

- Deng Hexuan 3036645662
- Feng Ruichen 3036662555
- Guo Youzhi 3036539136
- Rui Yicheng 3036662244
- Zhou Chuanyu 3036477833

All team members have contributed to design, implementation, and documentation.

---

## Game Description

The player assumes the role of a hero traversing a sequence of enemy encounters. Each level presents a unique enemy with distinct behavioural patterns. The player begins with a starter deck containing Strike, Dodge, Heal, and The Totem cards. Each turn, the player draws two cards, selects cards to play using keyboard navigation, and ends the turn to allow the enemy to act.

Victory in a battle rewards the player with a new skill chosen from a random selection of abilities not already owned. Defeat results in run termination unless the player has saved progress to a checkpoint. The game features three difficulty levels (Easy, Normal, Hard) affecting starting health, dodge effectiveness, enemy statistics, and total level count. Save data persists across sessions using a file-based storage system supporting three slots.

---

## Special Features

The Totem Dual Functionality: The Totem card provides two distinct effects. When played, it buffs the next strike with +1 damage and undodgeable property. Additionally, if the player would receive fatal damage while holding The Totem, a prompt appears allowing the player to shatter the card to gain 2 HP and survive.

Interactive Battle Interface: The terminal UI presents three panels displaying player and enemy status, a scrollable battle log, and a 2x4 grid representation of the player's hand with visual selection highlighting.

Enemy Variety and Behaviours: Four standard enemy types (Soldier, Warrior, Knight, Champion) plus a Hard-mode Boss each exhibit unique mechanics including Unstoppable state requiring two dodges, first-turn sleep, Heavy Smash, and Eight Trigrams Formation granting bonus dodges.

Active Skill System: The Sacrifice skill allows the player to lose 1 HP and draw three cards by pressing the Q key during their turn, creating a risk-reward trade-off.

Checkpoint Save During Pause: Pressing Esc or Space pauses the game and presents options to resume, save the current checkpoint to the active slot, or return to the lobby.

---

## In-Game Controls

Arrow Keys: Navigate menus and select cards in the hand grid

Enter: Confirm selection or play selected card

E: End the player turn early

Q: Activate Sacrifice skill (if acquired)

Esc / Space: Pause game (then R to resume, S to save checkpoint)

---

## How Coding Elements in Coding Requirements Supports Features in the Game

| Requirement | Implementation Location |
|-------------|------------------------|
| 1. Generation of random events | `Deck::shuffle()` for card draw randomness; random skill selection after enemy defeat; 50% chance for Eight Trigrams Formation dodge gain |
| 2. Data structures for storing data | `std::vector<Card*>` for hands and piles; `std::vector<Skill>` for player skills; `std::vector<std::string>` for battle logs |
| 3. Dynamic memory management | `CardPool` class using `std::unique_ptr<Card>` and a free list for card recycling; no memory leaks |
| 4. File input/output | `SaveLoad` class reads and writes `save_slot_N.txt` files containing difficulty, level, HP, and skill IDs |
| 5. Program codes in multiple files | 14 source files including `main.cpp`, `BattleUI.cpp`, `Card.cpp`, `Deck.cpp`, `Enemy.cpp`, `Player.cpp`, `Level.cpp`, `menu.cpp`, `HowToPlay.cpp`, `SaveLoad.cpp` and corresponding headers |
| 6. Multiple difficulty levels | Easy (8 HP, 25% dodge), Normal (6 HP, 15% dodge), Hard (5 HP, 5% dodge); Hard mode features a fifth boss level and increased enemy damage |

---

## Non-Standard Libraries

No external libraries are required. The implementation uses only standard C++17 libraries including iostream, fstream, vector, algorithm, random, chrono, thread, sstream, memory, ctime, and cstdlib. Platform-specific terminal control code exists within ConsoleUtils.h using preprocessor directives, but no additional installation is required.

---

## Compilation and Execution Instructions

Prerequisites:

- C++17 compiler (GCC 7 or later, Clang, or MinGW on Windows)
- Make utility (optional)

Compilation with Make:

make

If Make is not available, compile manually using:

g++ -Wall -std=c++17 -o game main.cpp Card.cpp Deck.cpp Enemy.cpp Player.cpp menu.cpp Level.cpp SaveLoad.cpp BattleUI.cpp HowToPlay.cpp

This produces an executable named game (or game.exe on Windows).

Execution: Run the compiled program with:

./game

If the game can't display properly, try enlarging the terminal window or decrease terminal font size (if possible), so the entirety of the game UI can be displayed.

Clean Build: To remove compiled objects and rebuild from scratch, execute:

make clean

Save File Location:

Save data is written to save_slot_1.txt, save_slot_2.txt, and save_slot_3.txt in the working directory. These files may be copied, moved, or deleted manually.
