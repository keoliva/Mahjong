#ifndef AIPLAYER_H
#define AIPLAYER_H
#include "Player.h"

class AIPlayer : public Player {
    public:
        AIPlayer();
        Tile *discardTile(int selected_index=0);
        void determineTileToDiscard();
        ~AIPlayer();
    private:
        int indexOfTileToDiscard;
};

#endif // AIPLAYER_H
