#include "Player.h"
using namespace std;

ostream &operator<<(std::ostream &os, enum wind w)
{
    if (w >= WINDS_COUNT || w < 0) return os << "???";
    return os << wind_strings[w];
}
bool Player::isDealer()
{
    return _wind == EAST;
}

Player::~Player()
{
    //dtor
}
