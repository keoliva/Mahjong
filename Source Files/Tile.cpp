#include "include/Tile.h"
#include <algorithm> // std::random_shuffle
#include <chrono>
#include <iostream>
#include <random>       // std::default_random_engine
#include <sstream>
using namespace std;

string SuitTile::get_val() const
{
    stringstream ss;
    ss << className <<"(" << suit << ")";
    return ss.str();
}
Tile *SuitTile::clone() const {
	return new SuitTile(className, suit);
}

bool const operator<(const SuitTile &a, const SuitTile &b)
{
    return (a.suit < b.suit);
}
bool const operator>(const SuitTile &a, const SuitTile &b)
{
    return (a.suit > b.suit);
}
bool SuitTile::inSequence(std::vector<SuitTile*> tiles)
{
    SuitTile *lastTile = NULL;
    for (SuitTile *tile : tiles) {
        if (lastTile) {
            if (tile->suit - lastTile->suit != 1) return false;
        }
        lastTile = tile;
    }
    return true;
}
string AttributeTile::get_val() const
{
    std::stringstream ss;
    ss << className <<"(" << attribute << ")";
    return ss.str();
}
Tile *AttributeTile::clone() const
{
	return new AttributeTile(className, type, attribute);
}


bool Tile::areEqual(const Tile &a, const Tile &b)
{
   return a.get_val() == b.get_val();
}

const int suitDuplicates = 4;
const int honorDuplicates = 4;
const int bonusDuplicates = 1;

const vector<string> dragonTypes = {"GREEN", "RED", "WHITE"};
const vector<string> windTypes = {"NORTH", "EAST", "WEST", "SOUTH"};
const vector<string> flowerTypes = {"PLUMB", "ORCHID", "CHRYSANTHEMUM", "BAMBOO"};
const vector<string> seasonTypes = {"SPRING", "SUMMER", "AUTUMN", "WINTER"};
void Tile::createTheTiles(tiles_t &pile)
{
    int suitRange = 10;
    // create the suit tiles
    for (int i = 0; i < suitDuplicates; i++) {
        for (int suit = 1; suit < suitRange; suit++) {
            pile.push_back(make_unique<BambooTile>(suit));
            pile.push_back(make_unique<CharacterTile>(suit));
            pile.push_back(make_unique<CircleTile>(suit));
        }
    }
    // create the honor tiles
    for (int i = 0; i < honorDuplicates; i++) {
        for (string type : dragonTypes)
            pile.push_back(make_unique<DragonTile>(type));
        for (string type : windTypes)
            pile.push_back(make_unique<WindTile>(type));
    }
    // create the bonus tiles
    for (int i = 0; i < bonusDuplicates; i++) {
        for (string type : flowerTypes)
            pile.push_back(make_unique<FlowerTile>(type));
        for (string type : seasonTypes)
            pile.push_back(make_unique<SeasonTile>(type));
    }
    // giving us a total of 144 tiles
    // shuffle the tiles
    int _size = pile.size();
    int j;
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator (seed);
    for (int i = _size-1; i > 0; i--) {
        uniform_int_distribution<int> dist(0, i);
        j = dist(generator); // random index from 0 to i
        iter_swap(pile.begin() + i, pile.begin() + j);
    }
}
