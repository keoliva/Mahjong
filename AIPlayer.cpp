#include "AIPlayer.h"
#include <algorithm>
#include <chrono>
#include <random>
using namespace std;

AIPlayer::AIPlayer()
{
    //ctor
}

Declaration AIPlayer::getDeclaration()
{
    // means a tile that's not yours was recently discarded
    // and Player::determineOptions(Tile*) was called before this method was called
    // and mapped a value to the keys PENG, CHI, KANG
    if (options.find(KANG) != options.end()) {
        return Declaration::NONE;
    } else {
        if (options.find(SMALL_MELDED_KANG) != options.end()) {
            return Declaration::SMALL_MELDED_KANG;
        } else if (options.find(CONCEALED_KANG) != options.end()) {
            return Declaration::CONCEALED_KANG;
        } else {
            return Declaration::NONE;
        }
    }
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
