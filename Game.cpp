#include "include/Game.h"
#include "include/AIPlayer.h"
#include "include/HumanPlayer.h"
#include <GL/glut.h>
#include <chrono>
#include <random>
using namespace std;
int Game::humanPlayerIndex = 0;
Game::Game()
{
    roundIsOver = false;
    rounds = 0;
    chooseDealer();
    curr_state.players[humanPlayerIndex] = new HumanPlayer();
    int i = (humanPlayerIndex + 1) % NUM_PLAYERS;
    for (; i != humanPlayerIndex; i = (i+1)%NUM_PLAYERS)
        curr_state.players[i] = new AIPlayer();
    updatePlayerWinds();

    turnManager = new Turn(this, getCurrentPlayer());
    tileDealer = new TileDealer(this);
    pile = Tile::createTheTiles();
}
void Game::update()
{
    turnManager->update();
}
void Game::chooseDealer()
{
    srand(time(NULL));
    curr_state.dealerReference = rand() % NUM_PLAYERS;
    curr_state.currPlayerReference = curr_state.dealerReference;
}
bool Game::finishedDealing()
{
    return tileDealer->finishedDealing;
}
void Game::dealTiles()
{
    tileDealer->deal();
}
void Game::cycleCurrentPlayer()
{
    curr_state.currPlayerReference++;
    curr_state.currPlayerReference %= NUM_PLAYERS;

    turnManager->setCurrentPlayer(getCurrentPlayer());
}
void Game::cycleDealer()
{
    curr_state.dealerReference++;
    curr_state.dealerReference %= NUM_PLAYERS;
    curr_state.currPlayerReference = curr_state.dealerReference;
}
void Game::takeFromWall(Player *player)
{
    Tile *tile;
    bool successfullyTookTile = false;
    while (!pile.empty()) {
        tile = pile.back();
        player->takeTile(tile);
        cout << "Tile being drawn: " << tile->get_val() << endl;
        pile.pop_back();
        if (tile->type == BONUS) {
            curr_discard = tile;
            continue;
        } else {
            successfullyTookTile = true;
            break;
        }
    }
    if (!successfullyTookTile) throw NoMoreTilesError(); return;
}

void Game::restart()
{
    if (!playExtraHand) {
        if (matchOver()) {
            chooseDealer();
            updatePlayerWinds();
        } else {
            // update round wind
            rounds = (rounds + 1) % NUM_ROUNDS;
            cycleDealer();
            updatePlayerWinds();
        }
    } else {
        playExtraHand = false;
        curr_state.currPlayerReference = curr_state.dealerReference;
    }
    roundIsOver = false;
    pile = Tile::createTheTiles();
    dealTiles();
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
    delete turnManager;
    delete tileDealer;
    for (Tile *tile : pile) {
        delete tile;
    }
    for (int i = 0; i < NUM_PLAYERS; i++)
        delete curr_state.players[i];
}
