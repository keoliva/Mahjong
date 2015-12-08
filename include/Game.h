#ifndef GAME_H
#define GAME_H
#include "Player.h"
#include <iostream>
#include <string>

struct state {
    Player *players[4]; // at 0: human player, 1: player to the right of human player and so on
    int dealerReference, currPlayerReference;
};

class Game
{
    public:
        state curr_state;
        Game();
        std::string getPrevailingWind(); // round wind
        int getTilesLeft();
        Player *getCurrentPlayer();
        bool roundOver();
        void restart(); // start new round
        virtual ~Game();
    protected:
    private:
        int rounds;
        std::vector<Tile*> pile;
        void chooseDealer();
        void init_state();
        void updatePlayerWinds();
        bool roundIsOver, playExtraHand; // holds if dealer wins or there's a draw
        bool matchOver();



};

#endif // GAME_H
