#ifndef HANDEVALUATOR_H
#define HANDEVALUATOR_H
#include "Tile.h"
#include <map>
#include <vector>
struct meld {
    std::vector<int> indicesInMelds, indicesInHand;
    int indexInHand;
};

enum MeldType {
    SINGLE=1, PAIR, PENG, KANG
};

class HandEvaluator
{
    public:
        HandEvaluator();
        void test();
        meld canDeclareMeldedPeng(std::vector<Tile*> hand, Tile *discardedTile);
        std::vector<meld> canDeclareMeldedChi(std::vector<Tile*> hand, Tile *discardedTile);
        meld canDeclareBigMeldedKang(std::vector<Tile*> hand, Tile *discardedTile);
        std::vector<meld> canDeclareSmallMeldedKang(std::vector<Tile*> melds, std::vector<Tile*> hand);
        std::vector<meld> canDeclareConcealedKang(std::vector<Tile*> hand);
        bool canDeclareMahjong(std::vector<Tile*> melds, std::vector<Tile*> hand);
        int getScore(std::vector<Tile*> melds, std::vector<Tile*> hand,
                    bool winBySelfDraw, bool winByDiscard);
        virtual ~HandEvaluator();
    protected:
    private:
        std::map<MeldType, std::vector<std::vector<int>>> handClassifications;
        std::vector<std::vector<int>> splitTiles(std::vector<Tile*> tiles,
                                                 bool (*eq)(const Tile &a, const Tile &b));
        int findMatchingTile(std::vector<Tile*> hand, Tile *tileToBeFound);
        std::vector<SuitTile*> insertIntoPossibleChi(std::vector<SuitTile*> tiles,
                                                                    SuitTile *tile);
};

#endif // HANDEVALUATOR_H
