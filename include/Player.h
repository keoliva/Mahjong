#ifndef PLAYER_H
#define PLAYER_H
#include "Tile.h"
#include <iostream>
#include <string>
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

        Player() : score(0) {};
        bool isDealer();
        //Player &operator=(const Player &other);
        ~Player() { std::cout << "deleting Player"; };
    protected:
    private:
        std::vector<Tile> *hand, *discards, *melds, *bonuses;

};

#endif // PLAYER_H
