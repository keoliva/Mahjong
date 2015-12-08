#include "Player.h"
using namespace std;

bool Player::isDealer()
{
    return _wind == EAST;
}

Player::~Player()
{
    //dtor
}
