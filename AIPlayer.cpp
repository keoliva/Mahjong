#include "AIPlayer.h"
#include <algorithm>
#include <chrono>
#include <random>
using namespace std;

AIPlayer::AIPlayer()
{
    //ctor
}

Tile *AIPlayer::discardTile(int selected_index)
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

AIPlayer::~AIPlayer()
{
    //dtor
}
