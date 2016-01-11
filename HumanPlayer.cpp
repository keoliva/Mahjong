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

HumanPlayer::~HumanPlayer()
{
    //dtor
}
