#include "Deck.h"

Deck::~Deck() {
    clear();
}

void Deck::clear() {
    for (Card* card : draw_pile) {
        g_cardPool.release(card);
    }
    draw_pile.clear();
    for (Card* card : discard_pile) {
        g_cardPool.release(card);
    }
    discard_pile.clear();
}

void Deck::initPlayerDeck() {
    clear();
    for (int i = 0; i < 5; i++) draw_pile.push_back(g_cardPool.acquire(CardType::SHA));
    for (int i = 0; i < 2; i++) draw_pile.push_back(g_cardPool.acquire(CardType::SHAN));
    for (int i = 0; i < 1; i++) draw_pile.push_back(g_cardPool.acquire(CardType::TAO));
    for (int i = 0; i < 1; i++) draw_pile.push_back(g_cardPool.acquire(CardType::TOTEM));
    shuffle();
}

void Deck::initEnemyDeck() {
    clear();
    for (int i = 0; i < 3; i++) draw_pile.push_back(g_cardPool.acquire(CardType::SHA));
    for (int i = 0; i < 3; i++) draw_pile.push_back(g_cardPool.acquire(CardType::SHAN));
    for (int i = 0; i < 1; i++) draw_pile.push_back(g_cardPool.acquire(CardType::TAO));
    shuffle();
}

void Deck::shuffle() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(draw_pile.begin(), draw_pile.end(), g);
}

Card* Deck::drawCard() {
    if (draw_pile.empty()) {
        draw_pile = discard_pile;
        discard_pile.clear();
        shuffle();
    }
    Card* c = draw_pile.back();
    draw_pile.pop_back();
    return c;
}

void Deck::discardCard(Card* c) {
    if (c) {
        discard_pile.push_back(c);
    }
}