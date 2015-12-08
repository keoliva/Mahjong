#include "Game.h"
#include <time.h>
#include <random>
#define NUM_ROUNDS 4
#define NUM_PLAYERS 4
#define POS(x) (x > 0) ? x : -x
using namespace std;
Game::Game()
{
    rounds = 0;
    chooseDealer();
    for (int i = 0; i < NUM_PLAYERS; i++)
        curr_state.players[i] = new Player();
    init_state();
}
void Game::chooseDealer()
{
    srand(time(NULL));
    curr_state.dealerReference = rand() % NUM_PLAYERS;
    curr_state.currPlayerReference = curr_state.dealerReference;
}
void Game::init_state()
{
    pile = Tile::createTheTiles();

}
void Game::restart()
{
    if (matchOver()) {
        chooseDealer();
    }
    if (!playExtraHand) {
        rounds = ++rounds % NUM_ROUNDS; // update wind
        curr_state.dealerReference = ++curr_state.dealerReference % NUM_PLAYERS; // update dealer
        updatePlayerWinds();
    } else {

    }
    playExtraHand = false;
    roundIsOver = false;
    init_state();
}
void Game::updatePlayerWinds()
{
    int dealerIndex = curr_state.dealerReference;
    int i = dealerIndex, playerWind = 0; // wind::EAST == 0
    do {
        curr_state.players[i]->_wind = static_cast<wind>(playerWind++);
        i = (i + 1) % NUM_PLAYERS;
    } while (i != dealerIndex);
}
Player *Game::getCurrentPlayer()
{
    return curr_state.players[curr_state.currPlayerReference];
}
string Game::getPrevailingWind()
{
    // 0 <= rounds < NUM_ROUNDS
    return wind_strings[rounds]; // static list defined in Player.h
}
int Game::getTilesLeft()
{
    return pile.size();
}
bool Game::roundOver()
{
    return roundIsOver;
}
bool Game::matchOver()
{
    return roundIsOver && getPrevailingWind() == "NORTH";
}

Game::~Game()
{
    for (Tile *tile : pile)
        delete tile;
    for (int i = 0; i < NUM_PLAYERS; i++)
        delete curr_state.players[i];
}
