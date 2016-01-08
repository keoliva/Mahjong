#ifndef PLAYER_H
#define PLAYER_H
#include "HandEvaluator.h"
#include "Tile.h"
#include <iostream>
#include <map>
#include <string>
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

class Player
{
    public:
        wind _wind;
        int score;
        std::vector<Tile*> hand, discards, melds, bonuses;
        Player();
        void takeTile(Tile *tile);
        virtual Tile *discardTile(int selected_index=0);
        bool isDealer();
        bool hasHandSize(int _size);
        void sortHand();
        std::map<MeldType, std::vector<meld>> getOptions(Tile *discardedTile=nullptr);
        virtual ~Player();
    private:
        HandEvaluator *handEvaluator;
        std::map<MeldType, std::vector<meld>> options;
};

#endif // PLAYER_H
