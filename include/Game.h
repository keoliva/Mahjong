#ifndef GAME_H
#define GAME_H
#include "Tile.h"
#include <iostream>
#include <string>
#include <vector>
enum wind {
    EAST, WEST, NORTH, SOUTH
};
class Game
{
    public:
        wind round;
        Game();
        std::string getRoundWind();
        virtual ~Game();
    protected:
    private:
        void init();

};

#endif // GAME_H
