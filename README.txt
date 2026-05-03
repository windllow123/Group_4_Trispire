Ray — A Roguelike Deckbuilder
=============================

A console card-battle roguelike that blends ideas from Slay the Spire and Sanguosha (Legend of the Three Kingdoms). Fight through five encounters, earn random skills after each win, and try to clear the run without dying.

Overview
--------

You play as a hero facing a sequence of unique enemies. Each victory grants a new skill and permanently improves your build until you lose or finish all five levels. Runs use permadeath-style stakes: when your HP hits zero, the run ends.

Cards
-----

Strike: Deals damage to the enemy (base 1; can be buffed). Subject to per-turn Strike limits and enemy Dodge.

Dodge: Blocks enemy Strike hits by spending Dodge cards from your hand (enemies may require two Dodges when Unstoppable rules apply).

Heal: Restores 1 HP, up to your maximum.

The Totem: Buffs your next Strike this round (+1 damage and cannot be dodged). If a hit would reduce you to 0 HP, you may instead discard The Totem to heal 2 HP and survive that blow (prompt appears when applicable).

Dodge is normally not played as an attack card unless you have the Dragon Gut skill (then Dodge can be used like Strike under the same Strike-per-turn limits).

Combat Flow
-----------

Player turn:
  - Draw 2 cards at the start of your turn (you also start the game with an opening draw).
  - Select cards with Left/Right, Enter to play, E to end the turn early.
  - With Sacrifice, press Q to activate it (lose 1 HP and draw 2 cards if allowed).
  - Strike uses are limited per turn (base 2, increased by Warscream).
  - At end of turn, if your hand size exceeds your current HP, you must discard down (interactive discard; Temperance can skip this phase if you played no Strikes).

Enemy turn:
  - The enemy regenerates (amount depends on difficulty), draws 1 card, then attacks with Strike logic and finally discards down so hand size does not exceed enemy HP.

Win / lose:
  - Win the battle by reducing the enemy's HP to 0.
  - Lose if your HP reaches 0.

Pause:
  - Pause returns you to a small menu: resume (R), save (S), or go back to the lobby (Esc or Space). Loading restores progress but note that enemy state and your hand are reset when loading.

Encounters (5 levels)
----------------------

1. Unknown Soldier — Standard fight; introduces the baseline rules.

2. Warrior — On the enemy's first attack turn, they activate Unstoppable messaging; from level 2 onward, enemy attacks generally require two Dodges to fully block (see Level.cpp and Enemy.cpp for exact flags).

3. Knight — Sleeps on the first enemy attack turn (no attack). On the second enemy attack turn, performs Heavy Smash (flavor); damage still follows the enemy's configured enemy_damage.

4. Champion — On the second enemy attack turn, activates Eight Trigrams Formation: each subsequent enemy turn has a 50% chance to add an extra Dodge to the enemy hand.

5. Boss — Double maximum HP compared to the usual formula for that level; otherwise behaves as a normal enemy without the earlier gimmicks.

Enemy HP scales with level: base HP from difficulty settings plus 2 x (level - 1); the boss (level 5) doubles that total. From level 3 onward, enemy strike damage can increase on Hard difficulty (see build).

Difficulty
----------

Selected in New Game (Easy / Normal / Hard). Player stats:

  Player HP:              Easy 8   Normal 6   Hard 5
  Dodge effectiveness:    x1.2     x1.0       x0.6
  Dodge chance:           25%      15%        5%

Enemy base HP, regeneration, and damage scaling are configured in Level.cpp together with the level index.

Skills (random reward after each victory)
------------------------------------------

1. Steel Cavalry — 50% chance that your Strike ignores the enemy's Dodge response.
2. Warscream — +1 Strike allowed per round.
3. Sacrifice — Active: lose 1 HP to draw 2 cards (Q outside of card play flow as implemented).
4. Temperance — If you played no Strike this turn, skip the discard phase.
5. Ambition — When you take damage from an enemy attack, draw 1 card (subject to hand rules).
6. Dragon Gut — Treat Dodge as Strike and Strike as Dodge where the implementation allows.

Duplicates are rerolled until you receive a skill you do not already have.

Controls (summary)
------------------

  Main menu: Arrow Up/Down, Enter to confirm.
  Difficulty: Arrow Left/Right, Enter; Esc or Space to go back.
  Battle: Arrow Left/Right to move selection among visible hand slots (up to 8 shown), Enter to play, E end turn, Q Sacrifice (if owned), pause key per ConsoleUtils / game prompts.

Build and run
-------------

Requirements:
  - A C++17 compiler (e.g. g++)
  - make (optional; recommended)

Compile:

  make

Or compile all translation units explicitly:

  g++ -Wall -std=c++17 -o game main.cpp Card.cpp Deck.cpp Enemy.cpp Player.cpp menu.cpp Level.cpp SaveLoad.cpp BattleUI.cpp

Run:

  ./game          (Linux / macOS / Git Bash)
  game.exe        (Windows CMD / PowerShell)

Use "make run" to build and run in one step (on Windows with MinGW, run game.exe if ./game.exe is not in PATH).

Other make targets:
  make clean   — remove objects and executable
  make rebuild — clean rebuild (useful if object files came from another OS)
  make help    — list Makefile targets

Project facts
-------------

  Language: C++17
  Style: Object-oriented modules (Player, Enemy, Deck, Level, UI, save/load)
  Interface: Terminal UI with keyboard input

Good luck on your climb.
