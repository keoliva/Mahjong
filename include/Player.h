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

enum PlayerStatus {
    DREW_TILE, DISCARDED_TILE
};
class Player
{
    HandEvaluator *handEvaluator;
    PlayerStatus curr_status;
    std::map<MeldType, std::vector<meld>> options;
    public:
        wind _wind;
        int score;
        std::vector<Tile*> hand, discards, melds, bonuses;
        Player();
        bool isDealer();
        bool hasFullHand(); // holds if player has a 13-tile hand
        void sortHand();
        std::map<MeldType, std::vector<meld>> getOptions(Tile *discardedTile=nullptr);
        void takeTile(Tile *tile);
        Tile *discardTile(int selected_index=0);
        ~Player();
};

#endif // PLAYER_H
