#include "Deck.h"

void Deck::initPlayerDeck() {
    draw_pile.clear();
    discard_pile.clear();
    for (int i = 0; i < 5; i++) draw_pile.emplace_back(CardType::SHA);
    for (int i = 0; i < 2; i++) draw_pile.emplace_back(CardType::SHAN);
    for (int i = 0; i < 1; i++) draw_pile.emplace_back(CardType::TAO);
    shuffle();
}

void Deck::initEnemyDeck() {
    draw_pile.clear();
    discard_pile.clear();
    for (int i = 0; i < 3; i++) draw_pile.emplace_back(CardType::SHA);
    for (int i = 0; i < 3; i++) draw_pile.emplace_back(CardType::SHAN);
    for (int i = 0; i < 1; i++) draw_pile.emplace_back(CardType::TAO);
    shuffle();
}

void Deck::shuffle() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(draw_pile.begin(), draw_pile.end(), g);
}

Card Deck::drawCard() {
    if (draw_pile.empty()) {
        draw_pile = discard_pile;
        discard_pile.clear();
        shuffle();
    }
    Card c = draw_pile.back();
    draw_pile.pop_back();
    return c;
}

void Deck::discardCard(const Card& c) {
    discard_pile.push_back(c);
}

void Deck::recycleDiscardPile() {
    draw_pile.insert(draw_pile.end(), discard_pile.begin(), discard_pile.end());
    discard_pile.clear();
    shuffle();
}
