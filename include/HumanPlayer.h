#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H
#include "Player.h"

enum class PlayerStatus {
    NONE,
    DREW_TILE, DISCARDED_TILE,
    DECLARING_MELD
};

class HumanPlayer : public Player
{
    public:
        bool providedInput;
        bool hasProvidedInput();
        HumanPlayer();
        void setStatus(PlayerStatus status) { curr_status = status; };
        Tile *discardTile(int selected_index=0);
        void takeTile(Tile *tile);
        void setDeclaration(std::pair<Declaration, int> declaration);
        void makeMeld(Tile *discardedTile=nullptr);
        PlayerStatus getStatus() { return curr_status; };
        ~HumanPlayer();
    private:
        PlayerStatus curr_status;

};

#endif // HUMANPLAYER_H
