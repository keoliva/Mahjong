#ifndef GAME_H
#define GAME_H
#include "Player.h"
#define NUM_ROUNDS 4
#define NUM_PLAYERS 4
#define INIT_NUM_TILES 144
#include <iostream>
#include <memory>
#include <string>

typedef struct state {
    Player *players[NUM_PLAYERS]; // at 0: human player, 1: player to the right of human player and so on
    int dealerReference, currPlayerReference;
} state;

class Game
{
    public:
        static int humanPlayerIndex;
        Game();
        std::string getPrevailingWind(); // round wind
        int getTilesLeft();
        Player *getCurrentPlayer();
        Player *getPlayer(int i);
        bool roundOver();
        void restart(); // start new round
        virtual ~Game();
    protected:
    private:
        state curr_state;
        int rounds;
        std::vector<Tile*> pile;
        void chooseDealer();
        void init_state();
        void updatePlayerWinds();
        bool roundIsOver, playExtraHand; // holds if dealer wins or there's a draw
        bool matchOver();
        void takeFromWall(Player *player);
};
#endif // GAME_H
