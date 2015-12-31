#ifndef HANDEVALUATOR_H
#define HANDEVALUATOR_H
#include <Tile.h>
#include <vector>
struct meld {
    std::vector<int> indicesInMelds, indicesInHand;
    int indexInHand;
};

class HandEvaluator
{
    public:
        HandEvaluator();
        void test();
        std::vector<meld> canDeclareSmallMeldedKang(std::vector<Tile*> melds, std::vector<Tile*> hand);
        std::vector<meld> canDeclareConcealedKang(std::vector<Tile*> hand);
        bool canDeclareMahjong(std::vector<Tile*> melds, std::vector<Tile*> hand);
        int getScore(std::vector<Tile*> melds, std::vector<Tile*> hand,
                    bool winBySelfDraw, bool winByDiscard);
        virtual ~HandEvaluator();
    protected:
    private:
        std::vector<std::vector<int>> splitTiles(std::vector<Tile*> tiles);
        int findMatchingTile(std::vector<Tile*> hand, Tile *tileToBeFound);
};

#endif // HANDEVALUATOR_H
