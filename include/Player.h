#ifndef PLAYER_H
#define PLAYER_H
#include "HandEvaluator.h"
#include "Tile.h"
#include <iostream>
#include <map>
#include <string>
#include <utility>
#define FULL_HAND_SIZE 13
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

enum class Declaration;
class Player
{
    public:
        wind _wind;
        int score;
        std::vector<Tile*> hand, discards, bonuses;
        std::vector<std::pair<MeldType, std::vector<Tile*>>> melds;
        static std::map<Declaration, MeldType> declarationToMeld;
        Player();
        virtual void takeTile(Tile *tile);
        virtual Tile *discardTile(int selected_index=0);
        void makeMeld(Tile *discardedTile=nullptr);
        bool isDealer();
        bool hasHandSize(int _size);
        void sortHand();
        void determineOptions(Tile *discardedTile=nullptr);
        std::pair<Declaration, int> getDeclaration();
        void setDeclaration(std::pair<Declaration, int> declaration);
        bool declarationIn(Declaration declarations[]);
        std::map<MeldType, std::vector<meld>> getOptions(Tile *discardedTile=nullptr);
        virtual ~Player();
    protected:
        HandEvaluator *handEvaluator;
        std::map<MeldType, std::vector<meld>> options;
        std::pair<Declaration, int> curr_declaration;
};

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
