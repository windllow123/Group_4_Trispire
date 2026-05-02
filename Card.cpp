#include "Card.h"

Card::Card(CardType t) : type(t) {
    switch (type) {
        case CardType::SHA: name = "Strike"; break;
        case CardType::SHAN: name = "Dodge"; break;
        case CardType::TAO: name = "Heal"; break;
    }
}

std::string Card::getName() const {
    return name;
}
