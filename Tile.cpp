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
string AttributeTile::get_val() const
{
    std::stringstream ss;
    ss << className <<"(" << attribute << ")";
    return ss.str();
}

const int suitDuplicates = 4;
const int honorDuplicates = 4;
const int bonusDuplicates = 1;

const vector<string> dragonTypes = {"GREEN", "RED", "WHITE"};
const vector<string> windTypes = {"NORTH", "EAST", "WEST", "SOUTH"};
const vector<string> flowerTypes = {"PLUMB", "ORCHID", "CHRYSANTHEMUM", "BAMBOO"};
const vector<string> seasonTypes = {"SPRING", "SUMMER", "AUTUMN", "WINTER"};
vector<Tile*> Tile::createTheTiles()
{
    vector<Tile*> tiles;
    int suitRange = 10;
    // create the suit tiles
    for (int i = 0; i < suitDuplicates; i++) {
        for (int suit = 1; suit < suitRange; suit++) {
            tiles.push_back(new BambooTile(suit));
            tiles.push_back(new CharacterTile(suit));
            tiles.push_back(new CircleTile(suit));
        }
    }
    // create the honor tiles
    for (int i = 0; i < honorDuplicates; i++) {
        for (string type : dragonTypes)
            tiles.push_back(new DragonTile(type));
        for (string type : windTypes)
            tiles.push_back(new WindTile(type));
    }
    // create the bonus tiles
    for (int i = 0; i < bonusDuplicates; i++) {
        for (string type : flowerTypes)
            tiles.push_back(new FlowerTile(type));
        for (string type : seasonTypes)
            tiles.push_back(new SeasonTile(type));
    }
    // giving us a total of 144 tiles
    // shuffle the tiles
    int _size = tiles.size();
    int j;
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator (seed);
    for (int i = _size-1; i > 0; i--) {
        uniform_int_distribution<int> dist(0, i);
        j = dist(generator); // random index from 0 to i
        iter_swap(tiles.begin() + i, tiles.begin() + j);
    }
    return tiles;
}


