#include "include/Player.h"
#include "include/Tile.h"
#include <algorithm>
#include <chrono>
#include <random>
using namespace std;
bool Player::isDealer()
{
    return _wind == EAST;
}
bool wantsToUseRecentlyDiscardedTile(Tile *tile)
{
    return false;
}
bool Player::hasFullHand()
{
    return hand.size() == FULL_HAND_SIZE;
}
void Player::takeTile(Tile *tile)
{
    if (tile->type == BONUS)
        bonuses.push_back(tile);
    else
        hand.push_back(tile);
}
Tile *Player::discardTile()
{
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator (seed);
    uniform_int_distribution<int> dist(0, hand.size()-1);
    int index = dist(generator); // random index from
    Tile *discardedTile = hand[index];
    discards.push_back(discardedTile);
    hand.erase(hand.begin() + index);

    return discardedTile;
}
void Player::sortHand()
{
    std::sort(hand.begin(), hand.end(), SortTiles());
}
