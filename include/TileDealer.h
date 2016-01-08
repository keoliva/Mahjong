#ifndef TILEDEALER_H
#define TILEDEALER_H
#include "Game.h"
#define NUM_PLAYERS 4
#define SIZE_DISTRIBUTIONS 4
class Game;
class TileDealer
{
    public:
        bool finishedDealing;
        TileDealer();
        TileDealer(Game *game);
        void deal();
        virtual ~TileDealer();
    private:
        Game *game_instance;
        int pieceCounter, playerCounter;
        int piecesSizes[SIZE_DISTRIBUTIONS], playerIndices[NUM_PLAYERS];
};

#endif // TILEDEALER_H
