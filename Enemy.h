#pragma once
#include <vector>
#include <string>
#include "Card.h"
#include "Deck.h"

struct Player;
class BattleUI;

// Represents an enemy character in battle
struct Enemy {
    int hp = 3;
    int max_hp = 3;
    std::string name = "Soldier";
    std::vector<Card*> hand;
    Deck deck;
    int shan_defense = 0;
    int enemy_damage = 1;
    int regen_amount = 0;
    bool canUseTao = true;
    bool sleepFirstTurn = false;
    bool heavySmashSecondTurn = false;
    int turnNumber = 0;
    bool activateUnstoppableFirstTurn = false;
    bool unstoppableAlways = false;
    bool getsEightTrigramsFormation = false;
    bool hasEightTrigrams = false;

    // Function: Constructs enemy, initializes deck and draws starting hand
    Enemy();
    
    // Function: Destroys enemy, returns all cards to deck
    ~Enemy();
    
    // Function: Draws initial 2 cards for the enemy's starting hand
    void startDraw();
    
    // Function: Draws 1 card from the enemy's deck
    void drawOne();
    
    // Function: Returns all cards in hand back to the deck
    void discardHandToDeck();
    
    // Function: Prints enemy's current HP and hand size to console
    void showStatus();
    
    // Function: Reduces enemy HP by the given damage amount
    // Input: dmg - amount of damage to apply
    void takeDamage(int dmg);
    
    // Function: Performs enemy's attack sequence against the player
    // Input: p - reference to the player being attacked
    // Input: ui - reference to battle UI for logging messages
    // Output: true if attack was performed, false if skipped
    bool attack(Player& p, BattleUI& ui);
    
    // Function: Discards cards until hand size does not exceed current HP
    void discardExcessCards();
    
    // Function: Enemy's card playing phase, prepares for incoming attacks
    void playCards();
    
    // Function: Sets enemy base stats according to difficulty level
    // Input: difficulty - 0 for easy, 1 for normal, 2 for hard
    void applyDifficulty(int difficulty);
    
    // Function: Heals enemy by regen_amount if below max HP
    void regenerate();
    
    // Function: Attempts to dodge an attack by using a Dodge card from hand
    // Output: true if a Dodge was used successfully, false otherwise
    bool respondToAttack();
};
