#include "Game.h"
#include <time.h>
#include <random>
using namespace std;
int Game::humanPlayerIndex = 0;
Game::Game()
{
    rounds = 0;
    chooseDealer();
    for (int i = 0; i < NUM_PLAYERS; i++)
        curr_state.players[i] = new Player();

    updatePlayerWinds();
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
    vector<Tile*>::iterator tileIt;
    Player *player;
    for (int i = 0; i < NUM_PLAYERS; i++) {
        player = curr_state.players[i];
        while (!player->hasFullHand())
            takeFromWall(player);
        player->sortHand();
    }
    cout << "finished dealing tiles..." << endl;
}
void Game::takeFromWall(Player *player)
{
    Tile *tile = pile.back();
    cout << "Tile being drawn: " << tile->get_val() << endl;
    player->takeTile(tile);
    pile.pop_back();
}
void Game::restart()
{
    if (matchOver()) {
        chooseDealer();
    }
    if (!playExtraHand) {
        // update round wind
        rounds = (rounds + 1) % NUM_ROUNDS;
        // update dealer
        curr_state.dealerReference = (curr_state.dealerReference + 1) % NUM_PLAYERS;
        curr_state.currPlayerReference = curr_state.dealerReference;
        updatePlayerWinds();
    } else {
        playExtraHand = false;
    }
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

Player *Game::getPlayer(int i)
{
    return curr_state.players[i];
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
    cout << "deleting game.." << endl;
    for (Tile *tile : pile) {
        delete tile;
    }
    for (int i = 0; i < NUM_PLAYERS; i++)
        delete curr_state.players[i];
}
