#ifndef GAME_H
#define GAME_H
#include "Player.h"
#include "Status.h"
#include "TileDealer.h"
#include "Turn.h"

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

struct NoMoreTilesError : public std::exception {};

class Turn;
class TileDealer;
class Game
{
    friend class Turn;
    friend class TileDealer;
    public:
        static int humanPlayerIndex;
        bool _finishedDealing;
        Game();
        Status getStatus() { return *curr_status; };
        void updateStatus(Status new_status) { *curr_status = new_status; };
        Tile *getDiscard() { return curr_discard; };
        std::string getPrevailingWind(); // round wind
        int getTilesLeft();
        Player *getCurrentPlayer();
        Player *getPlayer(int i);
        void update();
        void dealTiles();
        bool roundOver();
        bool finishedDealing();
        void restart(); // start new round
        virtual ~Game();
    private:
        Turn *turnManager;
        TileDealer *tileDealer;
        Tile *curr_discard;
        state curr_state;
        Status *curr_status;
        int rounds;
        std::vector<Tile*> pile;
        void setDiscard(Tile *tile) { curr_discard = tile; };
        void cycleCurrentPlayer();
        void cycleDealer();
        void chooseDealer();
        void updatePlayerWinds();
        bool roundIsOver, playExtraHand; // holds if dealer wins or there's a draw
        bool matchOver();
        void takeFromWall(Player *player);
};
#endif // GAME_H
