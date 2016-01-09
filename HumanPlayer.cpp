#include "HumanPlayer.h"

HumanPlayer::HumanPlayer()
{
    //ctor
}
void HumanPlayer::takeTile(Tile *tile)
{
    curr_status = DREW_TILE;
    Player::takeTile(tile);
}

Tile *HumanPlayer::discardTile(int selected_index)
{
    curr_status = DISCARDED_TILE;
    return Player::discardTile(selected_index);
}

bool HumanPlayer::statusIs(PlayerStatus status)
{
    return curr_status == status;
}

bool HumanPlayer::statusIn(PlayerStatus statuses[])
{
    int _size = sizeof(statuses)/sizeof(statuses[0]);
    for (int i = 0; i < _size; i++) {
        if (curr_status == statuses[i]) return true;
    }
    return false;
}

HumanPlayer::~HumanPlayer()
{
    //dtor
}
