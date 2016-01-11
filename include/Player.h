#ifndef PLAYER_H
#define PLAYER_H
#include "HandEvaluator.h"
#include "Tile.h"
#include <iostream>
#include <map>
#include <string>
#include <utility>

enum wind;
enum class Declaration;
class Player
{
    public:
        wind _wind;
        int score;
        std::vector<Tile*> hand, discards, melds, bonuses;
        Player();
        void takeTile(Tile *tile);
        virtual Tile *discardTile(int selected_index=0);
        void makeMeld();
        bool isDealer();
        bool hasHandSize(int _size);
        void sortHand();
        void determineOptions(Tile *discardedTile=nullptr);
        void std::pair<Declaration, int> getDeclaration();
        void setDeclaration(std::pair<Declaration, int> declaration);
        bool declarationIn(Declaration declarations[]);
        std::map<MeldType, std::vector<meld>> getOptions(Tile *discardedTile=nullptr);
        virtual ~Player();
    protected:
        HandEvaluator *handEvaluator;
        std::map<MeldType, std::vector<meld>> options;
        std::pair<Declaration, int> curr_declaration;
};
#define WINDS \
    X(EAST)\
    X(SOUTH)\
    X(WEST)\
    X(NORTH)

#define X(a) a,
enum wind { WINDS };// order prevailing winds rotate in
#undef X

#define X(a) #a,
static std::string wind_strings[] = { WINDS };
#undef X

#define DECLARATIONS \
    X(SMALL_MELDED_KANG) \
    X(CONCEALED_KANG) \
    X(MELDED_PENG) \
    X(MELDED_CHI) \
    X(MELDED_KANG) \
    X(MAHJONG)

#define X(a) a,
enum class Declaration {
    NONE,
    DECLARATIONS
};
#undef X

#endif // PLAYER_H
