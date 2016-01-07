#include "include/HandEvaluator.h"
#include <algorithm>
#include <cassert>
#include <exception>
#include <iostream>
using namespace std;
HandEvaluator::HandEvaluator()
{
    //ctor
}

vector<meld> HandEvaluator::canDeclareMeldedPeng(vector<Tile*> hand, Tile *discardedTile)
{
    meld melded_peng;
    vector<vector<int>> split_melds = splitTiles(hand, &Tile::areEqual);
    vector<vector<int>> concealed_pairs;
    concealed_pairs.reserve(handClassifications[PAIR].size() + handClassifications[PENG].size());
    concealed_pairs.insert(concealed_pairs.end(), handClassifications[PAIR].begin(), handClassifications[PAIR].end());
    concealed_pairs.insert(concealed_pairs.end(), handClassifications[PENG].begin(), handClassifications[PENG].end());
    for (vector<int> indices: concealed_pairs) {
        if (Tile::areEqual(*(hand[indices[0]]), *discardedTile)) {
            // just take the first two indices in case a concealed pair is technically within
            // a peng, where there is three of the tile
            melded_peng.indicesInHand.push_back(indices[0]);
            melded_peng.indicesInHand.push_back(indices[1]);
            break;
        }
    }
    vector<meld> peng (1, melded_peng);
    return peng;
}
// using binary search to insert the discarded tile into the sorted vector containing
// suitTileA and B
vector<SuitTile*> HandEvaluator::insertIntoPossibleChi(vector<SuitTile*> tiles, SuitTile *tile)
{
    vector<SuitTile*> ordered_tiles;
    SuitTile *suitTileA = tiles[0], *suitTileB = tiles[1];
    if (*tile < *suitTileB) {
        if (*tile < *suitTileA) {
            ordered_tiles = {tile, suitTileA, suitTileB};
        } else if (*tile > *suitTileA) {
            ordered_tiles = {suitTileA, tile, suitTileB};
        } else {
            throw -1;
        }
    } else if (*tile > *suitTileB) {
       ordered_tiles = {suitTileA, suitTileB, tile};
    } else {
        throw 1;
    }
    return ordered_tiles;
}
vector<meld> HandEvaluator::canDeclareMeldedChi(vector<Tile*> hand, Tile *discardedTile)
{
    SuitTile *discardedSuitTile = dynamic_cast<SuitTile*>(discardedTile);
    vector<meld> possibleChis;
    if (!discardedSuitTile) return possibleChis;
    auto eq = [](const Tile &a, const Tile &b) { return a.get_className() == b.get_className(); };
    vector<vector<int>> split_melds = splitTiles(hand, eq);
    int _size;
    Tile *tile;
    bool sameAsLowerSuit, sameAsHigherSuit;
    for (vector<int> indices : split_melds) {
        _size = indices.size();
        tile = hand[indices[0]];
        if (eq(*tile, *discardedTile) && tile->type == SUIT && _size >= 2) {
            for (int i = 0; i < _size-1; i+=(sameAsHigherSuit)?2:1) {
                sameAsLowerSuit = false; sameAsHigherSuit = false;
                meld melded_chi;
                vector<SuitTile*> tiles = { dynamic_cast<SuitTile*>(hand[i]),
                                            dynamic_cast<SuitTile*>(hand[i+1]) };
                try {
                    tiles = insertIntoPossibleChi(tiles, discardedSuitTile);
                } catch (int e) {
                    if (e == -1) { // discarded tile is the same as the tile at index i
                        try {
                            sameAsLowerSuit = true;
                            tiles.erase(tiles.begin());
                            tiles.insert(tiles.begin(), dynamic_cast<SuitTile*>(hand.at(i-1)));
                        } catch (std::exception &e) { continue; }
                    } else { // e == 1 discarded tile is the same as tile at index i+1
                        try {
                            sameAsHigherSuit = true;
                            tiles.push_back(dynamic_cast<SuitTile*>(hand.at(i+2)));
                        } catch (std::exception &e) { continue; }
                    }
                }
                if (!tiles.empty() && SuitTile::inSequence(tiles)) {
                    if (sameAsLowerSuit) {
                        melded_chi.indicesInHand.push_back(i-1);
                        melded_chi.indicesInHand.push_back(i+1);
                    } else if (sameAsHigherSuit) {
                        melded_chi.indicesInHand.push_back(i);
                        melded_chi.indicesInHand.push_back(i+2);
                    } else {
                        melded_chi.indicesInHand.push_back(i);
                        melded_chi.indicesInHand.push_back(i+1);
                    }
                    possibleChis.push_back(melded_chi);
                } else { continue; }
            }
        }
    }
    return possibleChis;
}
vector<meld> HandEvaluator::canDeclareBigMeldedKang(std::vector<Tile*> hand,
                                                    Tile *discardedTile)
{
    meld melded_kang;
    vector<vector<int>> split_melds = splitTiles(hand, &Tile::areEqual);
    vector<vector<int>> concealed_pengs = handClassifications[PENG];
    for (vector<int> indices: concealed_pengs) {
        if (Tile::areEqual(*(hand[indices[0]]), *discardedTile)) {
            melded_kang.indicesInHand = indices;
            break; // only four duplicates of a single tile
        }
    }
    vector<meld> kang (1, melded_kang);
    return kang;
}

vector<vector<int>> HandEvaluator::splitTiles(vector<Tile*> tiles, bool (*eq)(const Tile &a, const Tile &b))
{
    int _size = tiles.size();
    Tile *lastTile;
    if (_size > 0) lastTile = tiles[0];
    vector<int> meld;
    vector<vector<int>> split;
    for (int i = 0; i < _size; i++) {
        if (!(*eq)(*lastTile, *(tiles[i]))) {
            split.push_back(meld);
            handClassifications[static_cast<MeldType>(meld.size())].push_back(meld);
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
        if (Tile::areEqual(*tile, *tileToBeFound)) {
            return index;
        }
        index++;
    }
    return -1;
}
// see if there's a melded peng (three of the same tile) that can be promoted to a kang (four of the same)
vector<meld> HandEvaluator::canDeclareSmallMeldedKang(vector<Tile*> melds, vector<Tile*> hand)
{
    vector<meld> possibleKangs;
    vector<vector<int>> split_melds = splitTiles(melds, &Tile::areEqual);
    int kangIndex;
    for (vector<int> indices : split_melds) {
        if (indices.size() == 3) { // Peng (three of the same tile)
            kangIndex = findMatchingTile(hand, melds[indices[0]]);
            if (kangIndex > -1) {
                meld small_meld;
                small_meld.indicesInMelds = indices;
                small_meld.indexInHand = kangIndex;
                possibleKangs.push_back(small_meld);
            }
        }
    }
    return possibleKangs;
}

vector<meld> HandEvaluator::canDeclareConcealedKang(vector<Tile*> hand)
{
    vector<meld> possibleKangs;
    vector<vector<int>> split_hand = splitTiles(hand, &Tile::areEqual);
    for (vector<int> indices : split_hand) {
        if (indices.size() == 4) { // Kang (four of the same tile)
            meld concealed_kang;
            concealed_kang.indicesInHand = indices;
            possibleKangs.push_back(concealed_kang);
        }
    }
    return possibleKangs;
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
    bool (*equals)(const Tile &a, const Tile &b) = NULL;
    equals = &Tile::areEqual;
    vector<vector<int>> exp, res = splitTiles(tiles, equals);
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
    cout << "Running canDeclareMeldedPeng Tests..." << endl;
    meld _meld;
    _meld = canDeclareMeldedPeng(melds, tiles[0])[0];
    assert(_meld.indicesInHand.empty());
    cout << "Passed." << endl;;
    _meld = canDeclareMeldedPeng(melds, melds[0])[0];
    assert(!_meld.indicesInHand.empty());
    indices = {0, 1};
    assert(_meld.indicesInHand == indices);
    cout << "Passed." << endl;;
    cout << "Running canDeclareMeldedChi Tests..." << endl;
    hand = {new BambooTile(2), new BambooTile(3), new BambooTile(4), new BambooTile(5)};
    Tile *discardedTile = new BambooTile(6);
    vector<meld> chis;
    chis = canDeclareMeldedChi(hand, discardedTile);
    assert(chis.size() == 1);
    indices = {2, 3};
    _meld = chis[0];
    assert(_meld.indicesInHand == indices);
    cout << "Passed." << endl;
    discardedTile = new BambooTile(3);
    chis = canDeclareMeldedChi(hand, discardedTile);
    assert(chis.size() == 2);
    indices = {0, 2};
    _meld = chis[0];
    assert(_meld.indicesInHand == indices);
    indices = {2, 3};
    _meld = chis[1];
    assert(_meld.indicesInHand == indices);
    discardedTile = tiles[4];
    chis = canDeclareMeldedChi(hand, discardedTile);
    assert(chis.empty());
    cout << "Running canDeclareBigMeldedKang Tests..." << endl;
    discardedTile = new CharacterTile(5);
    kang = canDeclareBigMeldedKang(melds, discardedTile)[0];
    indices = {0, 1, 2};
    assert(kang.indicesInHand == indices);
    cout << "Passed." << endl;
    discardedTile = new DragonTile("RED");
    kang = canDeclareBigMeldedKang(melds, discardedTile)[0];
    assert(kang.indicesInHand.empty());
    cout << "Passed.";
    delete discardedTile;
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

HandEvaluator::~HandEvaluator()
{
    //dtor
}
