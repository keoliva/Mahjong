#include "include/Player.h"
#include "include/Tile.h"
#include <algorithm>
#include <chrono>
#include <random>
using namespace std;
Player::Player()
{
    score = 0;
    handEvaluator = new HandEvaluator();
}
Player::~Player()
{
    cout << "deleting player." << endl;
    delete handEvaluator;
}
bool Player::isDealer()
{
    return _wind == EAST;
}
map<MeldType, vector<meld>> Player::getOptions(Tile *discardedTile)
{
    map<MeldType, vector<meld>> options;
    if (discardedTile) {
        options[PENG] = handEvaluator->canDeclareMeldedPeng(hand, discardedTile);
        options[KANG] = handEvaluator->canDeclareBigMeldedKang(hand, discardedTile);
        options[CHI] = handEvaluator->canDeclareMeldedChi(hand, discardedTile);
    } else {
        options[SMALL_MELDED_KANG] = handEvaluator->canDeclareSmallMeldedKang(melds, hand);
        options[CONCEALED_KANG] = handEvaluator->canDeclareConcealedKang(hand);
    }
    return options;
}
bool Player::hasHandSize(int _size)
{
    return hand.size() == _size;
}
void Player::takeTile(Tile *tile)
{
    curr_status = DREW_TILE;
    if (tile->type == BONUS)
        bonuses.push_back(tile);
    else
        hand.push_back(tile);
}
Tile *Player::discardTile(int selected_index)
{
    curr_status = DISCARDED_TILE;
    Tile *discardedTile;
    int index;
    if (0 <= selected_index && selected_index < hand.size()) {
        index = selected_index;
        discardedTile = hand[selected_index];
    } else {
        unsigned seed = chrono::system_clock::now().time_since_epoch().count();
        default_random_engine generator (seed);
        uniform_int_distribution<int> dist(0, hand.size()-1);
        index = dist(generator); // random index from
        discardedTile = hand[index];
    }

    discards.push_back(discardedTile);
    hand.erase(hand.begin() + index);

    return discardedTile;
}
void Player::sortHand()
{
    std::sort(hand.begin(), hand.end(), SortTiles());
}
