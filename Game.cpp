#include "Game.h"
using namespace std;
Game::Game()
{
    round = EAST;
}

string Game::getRoundWind()
{
    switch (round) {
        case NORTH:
            return "NORTH"; break;
        case EAST:
            return "EAST"; break;
        case WEST:
            return "WEST"; break;
        case SOUTH:
            return "SOUTH"; break;
    }
}
Game::~Game()
{
    //dtor
}
