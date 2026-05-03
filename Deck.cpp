#include "Deck.h"

// Function: Destroys deck, releases all cards back to pool
Deck::~Deck() {
    clear();
}

// Function: Returns all cards to pool and empties both piles
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

// Function: Initializes player deck with 5 Strike, 2 Dodge, 1 Heal, 1 Totem
void Deck::initPlayerDeck() {
    clear();
    for (int i = 0; i < 5; i++) draw_pile.push_back(g_cardPool.acquire(CardType::SHA));
    for (int i = 0; i < 2; i++) draw_pile.push_back(g_cardPool.acquire(CardType::SHAN));
    for (int i = 0; i < 1; i++) draw_pile.push_back(g_cardPool.acquire(CardType::TAO));
    for (int i = 0; i < 1; i++) draw_pile.push_back(g_cardPool.acquire(CardType::TOTEM));
    shuffle();
}

// Function: Initializes enemy deck with 3 Strike, 3 Dodge, 1 Heal
void Deck::initEnemyDeck() {
    clear();
    for (int i = 0; i < 3; i++) draw_pile.push_back(g_cardPool.acquire(CardType::SHA));
    for (int i = 0; i < 3; i++) draw_pile.push_back(g_cardPool.acquire(CardType::SHAN));
    for (int i = 0; i < 1; i++) draw_pile.push_back(g_cardPool.acquire(CardType::TAO));
    shuffle();
}

// Function: Randomizes the order of cards in draw pile
void Deck::shuffle() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(draw_pile.begin(), draw_pile.end(), g);
}

// Function: Takes top card from draw pile, reshuffles discard if draw empty
// Output: pointer to the drawn card, nullptr only if both piles empty
Card* Deck::drawCard() {
    if (draw_pile.empty()) {
        draw_pile = discard_pile;
        discard_pile.clear();
        shuffle();
    }
    if (draw_pile.empty()) {
        return nullptr;
    }
    Card* c = draw_pile.back();
    draw_pile.pop_back();
    return c;
}

// Function: Places a used card into the discard pile
// Input: c - pointer to card being discarded
void Deck::discardCard(Card* c) {
    if (c) {
        discard_pile.push_back(c);
    }
}
