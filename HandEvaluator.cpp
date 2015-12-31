#include "include/HandEvaluator.h"
#include <algorithm>
#include <cassert>
#include <iostream>
using namespace std;
HandEvaluator::HandEvaluator()
{
    //ctor
}

void HandEvaluator::test()
{
    const vector<string> dragonTypes = {"GREEN", "RED", "WHITE"};
    const vector<string> windTypes = {"NORTH", "EAST", "WEST", "SOUTH"};
    vector<Tile*> tiles;
    for (int i = 0; i < 4; i++) {
        for (int suit = 1; suit < 2; suit++) {
            tiles.push_back(new BambooTile(suit));
        }
    }
    // create the honor tiles
    for (int i = 0; i < 4; i++) {
        for (string type : dragonTypes)
            tiles.push_back(new DragonTile(type));
    }
    sort(tiles.begin(), tiles.end(), SortTiles());
    for (Tile *tile : tiles) {
        cout << tile->get_val() << endl;
    }
    cout << "Running splitTiles Tests..." << endl;
    vector<vector<int>> res = splitTiles(tiles), exp;
    exp.push_back({0, 1, 2, 3});
    exp.push_back({4, 5, 6, 7});
    exp.push_back({8, 9, 10, 11});
    exp.push_back({12, 13, 14, 15});
    cout << ((res == exp)?"Passed.":"Failed.") << endl;
    cout << "Running findMatchingTile Tests..." << endl;
    Tile *tileToBeFound = new DragonTile("RED");
    cout << ((findMatchingTile(tiles, tileToBeFound) == 8)?"Passed.":"Failed.") << endl;
    tileToBeFound = new CharacterTile(1);
    cout << ((findMatchingTile(tiles, tileToBeFound) == -1)?"Passed.":"Failed.") << endl;
    tileToBeFound = new BambooTile(1);
    cout << ((findMatchingTile(tiles, tileToBeFound) == 0)?"Passed.":"Failed.") << endl;
    delete tileToBeFound;
    cout << "Running canDeclareSmallMeldedKang Tests..." << endl;
    vector<Tile*> hand = {new CharacterTile(4), new CharacterTile(5), new CircleTile(2), new BambooTile(7)};
    vector<Tile*> melds = { new CharacterTile(5), new CharacterTile(5),new CharacterTile(5), new CharacterTile(1),new CharacterTile(2), new CharacterTile(3) };
    sort(hand.begin(), hand.end(), SortTiles());
    vector<meld> vec = canDeclareSmallMeldedKang(melds, hand);
    assert(vec.size() == 1);
    meld kang = vec[0];
    vector<int> indices {0, 1, 2};
    cout << ((kang.indexInHand == 2 && kang.indicesInMelds == indices)?"Passed.":"Failed.") << endl;
    hand = {new CharacterTile(4), new CharacterTile(5), new CircleTile(2), new BambooTile(7)};
    cout << "Running canDeclareConcealedKang Tests..." << endl;
    vector<meld> kangs = canDeclareConcealedKang(hand);
    assert(kangs.size() == 0);
    hand.push_back(new CircleTile(2));
    hand.push_back(new CircleTile(2));
    hand.push_back(new CircleTile(2));
    sort(hand.begin(), hand.end(), SortTiles());
    kangs = canDeclareConcealedKang(hand);
    assert(kangs.size() == 1);
    kang = kangs[0];
    indices = {3, 4, 5, 6};
    cout << ((kang.indicesInHand == indices)?"Passed.":"Failed.") << endl;
    for (Tile *tile : hand) {
        delete tile;
    }
    for (Tile *tile : melds) {
        delete tile;
    }
    for (Tile *tile : tiles) {
        delete tile;
    }
}

vector<vector<int>> HandEvaluator::splitTiles(vector<Tile*> tiles)
{
    int _size = tiles.size();
    Tile *lastTile;
    if (_size > 0) lastTile = tiles[0];
    vector<int> meld;
    vector<vector<int>> split;
    for (int i = 0; i < _size; i++) {
        if (*lastTile != *(tiles[i])) {
            split.push_back(meld);
            meld.erase(meld.begin(), meld.end());
        }
        lastTile = tiles[i];
        meld.push_back(i);
    }
    split.push_back(meld);
    return split;
}

int HandEvaluator::findMatchingTile(vector<Tile*> hand, Tile *tileToBeFound)
{
    int index = 0;
    for (Tile *tile : hand) {
        if (*tile == *tileToBeFound) {
            return index;
        }
        index++;
    }
    return -1;
}
// see if there's a melded peng (three of the same tile) that can be promoted to a kang (four of the same)
vector<meld> HandEvaluator::canDeclareSmallMeldedKang(vector<Tile*> melds, vector<Tile*> hand)
{
    vector<meld> possibleMeldedKangs;
    vector<vector<int>> split_melds = splitTiles(melds);
    int kangIndex;
    for (vector<int> indices : split_melds) {
        if (indices.size() == 3) { // Peng (three of the same tile)
            kangIndex = findMatchingTile(hand, melds[indices[0]]);
            if (kangIndex > -1) {
                meld small_meld;
                small_meld.indicesInMelds = indices;
                small_meld.indexInHand = kangIndex;
                possibleMeldedKangs.push_back(small_meld);
            }
        }
    }
    return possibleMeldedKangs;
}

vector<meld> HandEvaluator::canDeclareConcealedKang(vector<Tile*> hand)
{
    vector<meld> possibleKangs;
    vector<vector<int>> split_hand = splitTiles(hand);
    for (vector<int> indices : split_hand) {
        if (indices.size() == 4) { // Kang (four of the same tile)
            meld concealed_kang;
            concealed_kang.indicesInHand = indices;
            possibleKangs.push_back(concealed_kang);
        }
    }
    return possibleKangs;
}

HandEvaluator::~HandEvaluator()
{
    //dtor
}
