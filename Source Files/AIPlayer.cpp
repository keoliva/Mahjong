#include "include/AIPlayer.h"
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
	player_tile_t discardedTile (hand[indexOfTileToDiscard]);
    std::cout << "Tile being discarded: " << discardedTile->get_val() << std::endl;
    discards.push_back(discardedTile);
    hand.erase(hand.begin() + indexOfTileToDiscard);

    return discardedTile.get();
}

AIPlayer::~AIPlayer() {
    //dtor
}
