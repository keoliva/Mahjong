#include "include/Player.h"
#include "include/Tile.h"
#include <algorithm>

Player::Player()
{
    score = 0;
    curr_declaration = std::make_pair(Declaration::NONE, -1);
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
std::pair<Declaration, int> Player::getDeclaration()
{
    return curr_declaration;
}
void Player::setDeclaration(std::pair<Declaration, int> declaration)
{
    curr_declaration = declaration;
}
bool Player::declarationIn(Declaration declarations[])
{
    int _size = sizeof(declarations)/sizeof(declarations[0]);
    Declaration declaration = curr_declaration.first;
    for (int i = 0; i < _size; i++) {
        if (declaration == declarations[i]) return true;
    }
    return false;
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
        sort(hand.begin(), hand.end(), SortTiles());
        return discardedTile;
    } else {
        return nullptr;
    }
}
void Player::makeMeld()
{
    Declaration declaration = curr_declaration.first;
    int indexIntoOptions = curr_declaration.second;

    meld meld_to_make;
    switch (declaration) {
        case Declaration::SMALL_MELDED_KANG: {
            meld_to_make = (options[SMALL_MELDED_KANG])[indexIntoOptions];
            int pengIndex = meld_to_make.indexInMeld;
            Tile *matching_tile = hand[meld_to_make.indexInHand];
            melds[pengIndex].first = SMALL_MELDED_KANG;
            melds[pengIndex].second.push_back(matching_tile);
            // tile that's moved to melds should no longer be in hand
            hand.erase(hand.begin() + meld_to_make.indexInHand);
            break;
        }
        case Declaration::CONCEALED_KANG:
            meld_to_make = options[CONCEALED_KANG][indexIntoOptions];
            std::vector<int> indicesInHand = meld_to_make.indicesInHand;
            std::vector<Tile*> tilesInNewMeld;
            for (int i : indicesInHand) {
                tilesInNewMeld.push_back(hand[i]);
                hand.erase(hand.begin() + i);
            }
            std::pair<MeldType, std::vector<Tile*>> new_meld (CONCEALED_KANG, tilesInNewMeld);
            melds.push_back(new_meld);
            break;
    }
}
void Player::sortHand()
{
    std::sort(hand.begin(), hand.end(), SortTiles());
}
