#pragma once
#include <string>

enum class CardType { SHA, SHAN, TAO };

struct Card {
    CardType type;
    std::string name;

    Card(CardType t);
    std::string getName() const;
};