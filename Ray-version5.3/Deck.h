#pragma once
#include <vector>
#include <algorithm>
#include <random>
#include "Card.h"

class Deck {
public:
    std::vector<Card> draw_pile;
    std::vector<Card> discard_pile;
    void recycleDiscardPile();
    void initPlayerDeck();
    void initEnemyDeck();
    Card drawCard();
    void discardCard(const Card& c);
    void shuffle();
};