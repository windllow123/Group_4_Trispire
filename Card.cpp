#include "Card.h"

// Function: Constructs card and initializes its name based on type
// Input: t - the card type
Card::Card(CardType t) : type(t) {
    reset(t);
}

// Function: Changes card type and updates name accordingly
// Input: t - the new card type to set
void Card::reset(CardType t) {
    type = t;
    switch (type) {
        case CardType::SHA: name = "Strike"; break;
        case CardType::SHAN: name = "Dodge"; break;
        case CardType::TAO: name = "Heal"; break;
        case CardType::TOTEM: name = "The Totem"; break;
    }
}

// Function: Gets the display name of the card
// Output: the card's name string
std::string Card::getName() const {
    return name;
}

// Function: Acquires a card from pool or creates new if none available
// Input: t - type of card to acquire
// Output: pointer to the card
Card* CardPool::acquire(CardType t) {
    if (!free_cards.empty()) {
        Card* card = free_cards.back();
        free_cards.pop_back();
        card->reset(t);
        return card;
    }
    owned_cards.push_back(std::make_unique<Card>(t));
    return owned_cards.back().get();
}

// Function: Returns a card to the free pool for reuse
// Input: card - pointer to the card to release
void CardPool::release(Card* card) {
    if (card) {
        free_cards.push_back(card);
    }
}

CardPool g_cardPool;
