#pragma once
#include <string>
#include <vector>
#include <memory>

enum class CardType { SHA, SHAN, TAO, TOTEM };

struct Card {
    CardType type;
    std::string name;

    Card(CardType t);
    void reset(CardType t);
    std::string getName() const;
};

class CardPool {
public:
    Card* acquire(CardType t);
    void release(Card* card);

private:
    std::vector<std::unique_ptr<Card>> owned_cards;
    std::vector<Card*> free_cards;
};

extern CardPool g_cardPool;