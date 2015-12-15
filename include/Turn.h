#ifndef TURN_H
#define TURN_H
#include "Game.h"
class Game;
class Turn
{
    public:
        Turn() {};
        Turn(Game *game, Player *player) : game_instance(game), curr_player(player) {};
        void setCurrentPlayer(Player *player) { curr_player = player; };
        void init();
        void drawTile();
        void discardTile();
        void declareSmallMeldedKang();
        void declareConcealedKang();
    protected:
    private:
        Game *game_instance;
        Player *curr_player;
};

#endif // TURN_H
