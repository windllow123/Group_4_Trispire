#pragma once
#include <vector>
#include "Card.h"
#include "Skill.h"
#include "Deck.h"

struct Enemy;

// Represents the player character with all stats, hand, skills, and deck
struct Player {
    static const int MAX_HAND_SIZE = 8;
    int hp = 5;
    int max_hp = 5;
    int max_sha_per_turn = 2;
    int current_sha_used = 0;
    int shan_defense = 0;
    double shan_effectiveness = 1.0;
    double dodge_chance = 0.0;

    std::vector<Card*> hand;
    std::vector<Skill> skills;
    Deck deck;
    bool nextStrikeIgnoreDodge = false;
    int nextStrikeBonusDamage = 0;

    // Function: Constructs player, initializes deck and draws starting hand
    Player();
    
    // Function: Destroys player, returns all hand cards to deck
    ~Player();
    
    // Function: Returns all cards in hand back to the deck
    void discardHandToDeck();
    
    // Function: Draws initial 3 cards at game start
    void startDraw();
    
    // Function: Draws 2 cards at the start of each turn
    void drawTwo();
    
    // Function: Draws 3 cards for Sacrifice skill
    void drawThree();
    
    // Function: Resets number of strikes used to 0 for new turn
    void resetShaCount();
    
    // Function: Clears temporary round effects like bonus damage
    void resetRoundEffects();
    
    // Function: Displays all cards currently in hand
    void showHand();
    
    // Function: Displays player's current HP and hand size
    void showStatus();
    
    // Function: Adds a skill to player and applies its effects
    // Input: s - the skill to add
    void addSkill(const Skill& s);
    
    // Function: Checks if player has a specific skill by name
    // Input: skillName - name of the skill to check
    // Output: true if player has the skill
    bool hasSkill(const std::string& skillName) const;
    
    // Function: Recalculates stats based on current skills
    void applySkillEffects();
    
    // Function: Sets player base stats according to difficulty
    // Input: difficulty - 0 for easy, 1 for normal, 2 for hard
    void applyDifficulty(int difficulty);
    
    // Function: Resets deck and hand for entering a new level
    void resetDeckAndHandForLevelEntry();
    
    // Function: Uses Sacrifice skill to lose 1 HP and draw 3 cards
    // Output: true if sacrifice was successful
    bool useSkillKuRou();
    
    // Function: Discards excess cards if hand exceeds maximum size
    void enforceHandLimit();
    
    // Function: Interactive discard phase, discards until hand <= HP
    // Input: enemy - reference to enemy for display
    // Input: returnToLobby - set to true if player pauses
    // Output: true if discard was needed
    bool discardExcessCards(Enemy& enemy, bool& returnToLobby);
    
    // Function: Checks if player has a card of specific type in hand
    // Input: type - the card type to look for
    // Output: true if at least one card of that type exists
    bool hasCard(CardType type) const;
    
    // Function: Removes and discards first card of given type from hand
    // Input: type - the card type to remove
    // Output: true if a card was found and removed
    bool removeCard(CardType type);
    
    // Function: Handles player response to enemy attack using Dodge cards
    // Input: requiredShan - number of Dodge cards needed to block
    // Output: true if attack was dodged successfully
    bool respondToAttack(int requiredShan = 1);
};
