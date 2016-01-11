#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H
#include "Player.h"

enum PlayerStatus {
    DREW_TILE, DISCARDED_TILE,
    NOT_DECLARING_MELD,
    DECLARING_CONCEALED_KANG, DECLARING_SMALL_MELDED_KANG
};

class HumanPlayer : public Player
{
    public:
        HumanPlayer();
        Tile *discardTile(int selected_index=0);
        void takeTile(Tile *tile);
        ~HumanPlayer();
    private:
        PlayerStatus curr_status;
};

#endif // HUMANPLAYER_H
