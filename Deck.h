#pragma once
#include <vector>
#include <algorithm>
#include <random>
#include "Card.h"

// Manages a deck of cards with draw and discard piles
class Deck {
public:
    std::vector<Card*> draw_pile;
    std::vector<Card*> discard_pile;

    // Function: Destroys deck, releases all cards back to the pool
    ~Deck();
    
    // Function: Releases all cards and clears both draw and discard piles
    void clear();
    
    // Function: Fills deck with player's starting card distribution and shuffles
    void initPlayerDeck();
    
    // Function: Fills deck with enemy's starting card distribution and shuffles
    void initEnemyDeck();
    
    // Function: Draws a card from draw pile, recycles discard pile if needed
    // Output: pointer to drawn card, or nullptr if deck completely empty
    Card* drawCard();
    
    // Function: Adds a card to the discard pile
    // Input: c - pointer to the card to discard
    void discardCard(Card* c);
    
    // Function: Randomly shuffles the draw pile
    void shuffle();
};
