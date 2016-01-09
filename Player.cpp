#include "include/Player.h"
#include "include/Tile.h"
#include <algorithm>

Player::Player()
{
    score = 0;
    handEvaluator = new HandEvaluator();
}
Player::~Player()
{
    std::cout << "deleting player." << std::endl;
    delete handEvaluator;
}
bool Player::isDealer()
{
    return _wind == EAST;
}
void Player::determineOptions(Tile *discardedTile)
{
    this->options.clear();
    std::map<MeldType, std::vector<meld>> options;
    if (discardedTile) {
        options[PENG] = handEvaluator->canDeclareMeldedPeng(hand, discardedTile);
        options[KANG] = handEvaluator->canDeclareBigMeldedKang(hand, discardedTile);
        options[CHI] = handEvaluator->canDeclareMeldedChi(hand, discardedTile);
    } else {
        options[SMALL_MELDED_KANG] = handEvaluator->canDeclareSmallMeldedKang(melds, hand);
        options[CONCEALED_KANG] = handEvaluator->canDeclareConcealedKang(hand);
    }
    this->options = options;
}
std::map<MeldType, std::vector<meld>> Player::getOptions(Tile *discardedTile)
{
    return options;
}
bool Player::hasHandSize(int _size)
{
    return hand.size() == _size;
}
void Player::takeTile(Tile *tile)
{
    if (tile->type == BONUS)
        bonuses.push_back(tile);
    else
        hand.push_back(tile);
}
Tile *Player::discardTile(int selected_index)
{
    if (0 <= selected_index && selected_index < hand.size()) {
        int index = selected_index;
        Tile *discardedTile = hand[selected_index];

        discards.push_back(discardedTile);
        hand.erase(hand.begin() + index);
        return discardedTile;
    } else {
        return nullptr;
    }
}
void Player::sortHand()
{
    std::sort(hand.begin(), hand.end(), SortTiles());
}
