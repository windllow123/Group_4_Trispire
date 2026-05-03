#pragma once
#include <string>
#include <vector>
#include <memory>

enum class CardType { SHA, SHAN, TAO, TOTEM };

// Represents a single card in the game
struct Card {
    CardType type;
    std::string name;

    // Function: Constructs a new card with the specified type
    // Input: t - the card type to create
    Card(CardType t);
    
    // Function: Resets card to a new type and updates its name
    // Input: t - the new card type
    void reset(CardType t);
    
    // Function: Returns the display name of the card
    // Output: string containing the card's name
    std::string getName() const;
};

// Object pool for efficient card memory management
class CardPool {
public:
    // Function: Gets a card of the specified type from the pool or creates a new one
    // Input: t - the card type needed
    // Output: pointer to the card
    Card* acquire(CardType t);
    
    // Function: Returns a card to the free list for reuse
    // Input: card - pointer to the card to release
    void release(Card* card);

private:
    std::vector<std::unique_ptr<Card>> owned_cards;
    std::vector<Card*> free_cards;
};

extern CardPool g_cardPool;
