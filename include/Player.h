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
enum wind {
    WINDS // order prevailing winds rotate in};
    WINDS_COUNT
 };
#undef X

#define X(a) #a,
static std::string wind_strings[] = { WINDS };
#undef X

static std::ostream &operator<<(std::ostream &os, enum wind w);
class Player
{
    public:
        wind _wind;
        std::vector<Tile> hand, discards, melds, bonuses;
        Player() {};
        bool isDealer();
        virtual ~Player();
    protected:
    private:

};

#endif // PLAYER_H
