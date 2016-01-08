#include "TileDealer.h"

TileDealer::TileDealer(Game *game)
{
    finishedDealing = false;
    game_instance = game;
    pieceCounter = 0;
    playerCounter = 0;

    piecesSizes[0] = 4;
    piecesSizes[1] = 4;
    piecesSizes[2] = 4;
    piecesSizes[3] = 1; // first four pieces will be dealt to each player, then another four, and so on
    for (int i = 0; i < NUM_PLAYERS; i++) {
        playerIndices[i] = (game->curr_state.dealerReference + i) % NUM_PLAYERS;
    }
}

void TileDealer::deal()
{
    int numPieces;
    if (playerCounter == NUM_PLAYERS) {
        playerCounter %= NUM_PLAYERS;
        if (pieceCounter+1 == SIZE_DISTRIBUTIONS) {
            pieceCounter %= SIZE_DISTRIBUTIONS;
            finishedDealing = true;
            return;
        } else {
            numPieces = piecesSizes[++pieceCounter];
        }
    } else {
        numPieces = piecesSizes[pieceCounter];
    }
    Player *player = game_instance->getPlayer(playerIndices[playerCounter++]);
    int newSize = player->hand.size() + numPieces;
    while (!player->hasHandSize(newSize)) {
        game_instance->takeFromWall(player);
    }
    player->sortHand();
}

TileDealer::~TileDealer()
{
    //dtor
}
