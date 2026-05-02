#include "Card.h"

Card::Card(CardType t) : type(t) {
    reset(t);
}

void Card::reset(CardType t) {
    type = t;
    switch (type) {
        case CardType::SHA: name = "Strike"; break;
        case CardType::SHAN: name = "Dodge"; break;
        case CardType::TAO: name = "Heal"; break;
        case CardType::TOTEM: name = "The Totem"; break;
    }
}

std::string Card::getName() const {
    return name;
}

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

void CardPool::release(Card* card) {
    if (card) {
        free_cards.push_back(card);
    }
}

CardPool g_cardPool;
