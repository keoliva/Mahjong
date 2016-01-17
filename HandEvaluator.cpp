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
            melded_peng.indicesInHand.insert(melded_peng.indicesInHand.begin(), indices.begin(), indices.begin() + 2);
            break;
        }
    }
    vector<meld> peng (1, melded_peng);
    return peng;
}

struct myclass {
  SuitTile &discardedTile;
  vector<Tile*> tiles;
  myclass(SuitTile &discard, vector<Tile*> _tiles) : discardedTile(discard),
                                                        tiles(_tiles) {};
  bool operator() (int i,int j) const {
    SuitTile &a = (i == -1)?(discardedTile):*(dynamic_cast<SuitTile*>(tiles[i]));
    SuitTile &b = (j == -1)?(discardedTile):*(dynamic_cast<SuitTile*>(tiles[j]));
    return a < b;
  };
};

vector<meld> HandEvaluator::canDeclareMeldedChi(vector<Tile*> hand, Tile *discardedTile)
{
    SuitTile *discardedSuitTile = dynamic_cast<SuitTile*>(discardedTile);
    vector<meld> possibleChis;
    if (!discardedSuitTile) return possibleChis;
    bool (*eq)(const Tile &a, const Tile &b) = [](const Tile &a, const Tile &b) {
        return a.get_className() == b.get_className();
    };
    vector<vector<int>> split_melds = splitTiles(hand, eq);
    vector<SuitTile*> tiles;
    vector<int> suit_indices;
    int _size, i;
    Tile *tile;
    bool sameAsHigherSuit = false;
    for (vector<int> indices : split_melds) {
        _size = indices.size();
        tile = hand[indices[0]];
        if ((*eq)(*tile, *discardedTile) && tile->type == SUIT && _size >= 2) {
            for (int j = 0; j <= _size-2; j+=(sameAsHigherSuit)?2:1) {
                i = indices[j];
                sameAsHigherSuit = false;
                                meld melded_chi;
                tiles.clear();
                tiles.push_back(discardedSuitTile);
                suit_indices.clear();
                suit_indices.push_back(-1); // -1 will signify the discardedSuitTile
                if (Tile::areEqual(*(hand[i]), *discardedTile)) {
                    continue;
                } else {
                    suit_indices.push_back(i);
                }
                tiles.push_back(dynamic_cast<SuitTile*>(hand[i]));
                if (Tile::areEqual(*(hand[i+1]), *discardedTile)) {
                    sameAsHigherSuit = true;
                    tiles.push_back(dynamic_cast<SuitTile*>(hand[i+2]));
                    suit_indices.push_back(i+2);
                } else {
                    tiles.push_back(dynamic_cast<SuitTile*>(hand[i+1]));
                    suit_indices.push_back(i+1);
                }
                // sort the tiles but keep track of the indices
                sort(suit_indices.begin(), suit_indices.end(),
                    [discardedSuitTile, hand](int i, int j)->bool {
                        SuitTile &a = (i == -1)?(*discardedSuitTile):
                                        *(dynamic_cast<SuitTile*>(hand[i]));
                        SuitTile &b = (j == -1)?(*discardedSuitTile):
                                        *(dynamic_cast<SuitTile*>(hand[j]));
                        return a < b;
                });
                sort(tiles.begin(), tiles.end(),
                    [](const SuitTile *a, const SuitTile *b)->bool {
                        return *a < *b;
                    }
                );
                if (SuitTile::inSequence(tiles)) {
                    melded_chi.indicesInHand = suit_indices;
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
            if (eq == &Tile::areEqual)
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
vector<meld> HandEvaluator::canDeclareSmallMeldedKang(vector<pair<MeldType, vector<Tile*>>> melds,
                                                        vector<Tile*> hand)
{
    vector<meld> possibleKangs;
    int kangIndex, i = 0;
    for (pair<MeldType, vector<Tile*>> _meld : melds) {
        if (_meld.first == PENG) { // Peng (three of the same tile)
            kangIndex = findMatchingTile(hand, _meld.second[0]);
            if (kangIndex > -1) {
                meld small_meld;
                small_meld.indexInMeld = i;
                small_meld.indexInHand = kangIndex;
                possibleKangs.push_back(small_meld);
            }
        }
        i++;
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
    cout << "Running splitTiles Tests..." << endl;
    vector<Tile*> hand0 = {new BambooTile(1), new BambooTile(9), new CharacterTile(2), new CharacterTile(3), new CharacterTile(4), new CharacterTile(5)};
    bool (*eq)(const Tile &a, const Tile &b) = [](const Tile &a, const Tile &b) {
        return a.get_className() == b.get_className();
    };
    vector<vector<int>> split_by_class = splitTiles(hand0, eq);
    assert(split_by_class.size() == 2);
    cout << "Passed." << endl;
    vector<int> b_indices = {0, 1};
    assert(split_by_class[0] == b_indices);
    cout << "Passed." << endl;
    vector<int> c_indices = {2, 3, 4, 5};
    assert(split_by_class[1] == c_indices);
    cout << "Passed." << endl;
    vector<vector<int>> split_melds = splitTiles(hand0, eq);
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
    vector<pair<MeldType, vector<Tile*>>> _melds = {
        pair<MeldType, vector<Tile*>> (PENG, {new CharacterTile(5), new CharacterTile(5),new CharacterTile(5)}),
        pair<MeldType, vector<Tile*>> (CHI, {new CharacterTile(1),new CharacterTile(2), new CharacterTile(3)}) };
    sort(hand.begin(), hand.end(), SortTiles());
    vector<meld> vec = canDeclareSmallMeldedKang(_melds, hand);
    assert(vec.size() == 1);
    meld kang = vec[0];
    cout << ((kang.indexInHand == 2 && kang.indexInMeld == 0)?"Passed.":"Failed.") << endl;
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
    vector<int> indices = {3, 4, 5, 6};
    cout << ((kang.indicesInHand == indices)?"Passed.":"Failed.") << endl;
    cout << "Running canDeclareMeldedPeng Tests..." << endl;
    meld _meld;
    vector<Tile*> melds = {new CharacterTile(5), new CharacterTile(5),new CharacterTile(5),
                    new CharacterTile(1),new CharacterTile(2), new CharacterTile(3)};
    _meld = canDeclareMeldedPeng(melds, tiles[0])[0];
    assert(_meld.indicesInHand.empty());
    cout << "Passed." << endl;;
    _meld = canDeclareMeldedPeng(melds, melds[0])[0];
    assert(!_meld.indicesInHand.empty());
    indices = {0, 1};
    assert(_meld.indicesInHand == indices);
    cout << "Passed." << endl;;
    cout << "Running canDeclareMeldedChi Tests..." << endl;
    /**vector<Tile*> hand0 = {new BambooTile(1), new BambooTile(9), new CharacterTile(2),
    new CharacterTile(3), new CharacterTile(4), new CharacterTile(5)};*/

    Tile *discardedTile = new CharacterTile(6);
    vector<meld> chis;
    chis = canDeclareMeldedChi(hand0, discardedTile);
    cout << "SIZE: " << chis.size() << endl;
    assert(chis.size() == 1);
    indices = {4, 5, -1};
    _meld = chis[0];
    assert(_meld.indicesInHand == indices);
    cout << "Passed." << endl;
    discardedTile = new CharacterTile(3);
    chis = canDeclareMeldedChi(hand0, discardedTile);
    assert(chis.size() == 2);
    indices = {2, -1, 4};
    _meld = chis[0];
    assert(_meld.indicesInHand == indices);
    indices = {-1, 4, 5};
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
    for (Tile *tile : hand0) {
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
