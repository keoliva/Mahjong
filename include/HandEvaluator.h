#ifndef HANDEVALUATOR_H
#define HANDEVALUATOR_H
#include "Tile.h"
#include <map>
#include <vector>
#define MELDS \
    X(SINGLE) \
    X(PAIR) \
    X(PENG) \
    X(KANG) \
    X(SMALL_MELDED_KANG) \
    X(CONCEALED_KANG) \
    X(CHI) \
	X(MAHJONG)

#define X(a) a,
enum MeldType {
    NONE=0,
    MELDS
};
#undef X


#define X(a) #a,
static std:: string meld_strings[] = { "NONE", MELDS };
#undef X

struct meld {
    std::vector<int> indicesInHand;
    int indexInHand, indexInMeld;
};

class HandEvaluator
{
    public:
        HandEvaluator();
        void test();
        std::vector<meld> canDeclareMeldedPeng(std::vector<Tile*> hand, Tile *discardedTile);
        std::vector<meld> canDeclareMeldedChi(std::vector<Tile*> hand, Tile *discardedTile);
        std::vector<meld> canDeclareBigMeldedKang(std::vector<Tile*> hand, Tile *discardedTile);
        std::vector<meld> canDeclareSmallMeldedKang(std::vector<
                                                    std::pair<MeldType, std::vector<Tile*>>
                                                    > melds, std::vector<Tile*> hand);
        std::vector<meld> canDeclareConcealedKang(std::vector<Tile*> hand);
        bool canDeclareMahjong(std::vector<Tile*> hand);
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
