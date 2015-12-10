#include "include/Player.h"
#include "include/Tile.h"
#include <algorithm>
using namespace std;

bool Player::isDealer()
{
    return _wind == EAST;
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
void Player::sortHand()
{
    sort(hand.begin(), hand.end(), SortTiles());
}
