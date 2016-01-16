#include "AIPlayer.h"
#include <algorithm>
#include <chrono>
#include <random>
using namespace std;

AIPlayer::AIPlayer() {
    //ctor
}

void AIPlayer::determineTileToDiscard() {
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator (seed);
    uniform_int_distribution<int> dist(0, hand.size()-1);
    indexOfTileToDiscard = dist(generator); // random index into hand
}

Tile *AIPlayer::discardTile(int selected_index) {
    Tile *discardedTile = hand[indexOfTileToDiscard];

    discards.push_back(discardedTile);
    hand.erase(hand.begin() + indexOfTileToDiscard);

    return discardedTile;
}

AIPlayer::~AIPlayer() {
    //dtor
}
