#include "Game.h"
#include <chrono>
#include <random>
using namespace std;

int Game::humanPlayerIndex = 0;
Game::Game()
{
    roundIsOver = false;
    rounds = 0;
    chooseDealer();
    for (int i = 0; i < NUM_PLAYERS; i++)
        curr_state.players[i] = new Player();
    updatePlayerWinds();
    init_state();
}
void Game::start()
{
    cout << "starting game..." << endl;
    turnManager = new Turn(this, getCurrentPlayer());
    turnManager->init();
}
void Game::chooseDealer()
{
    // index into curr_state.players
    srand(time(NULL));
    curr_state.dealerReference = rand() % NUM_PLAYERS;
    curr_state.currPlayerReference = curr_state.dealerReference;
}
void Game::init_state()
{
    pile = Tile::createTheTiles();
    Player *player;
    for (int i = 0; i < NUM_PLAYERS; i++) {
        player = curr_state.players[i];
        while (!player->hasFullHand())
            takeFromWall(player);
        player->sortHand();
    }
    cout << "finished dealing tiles..." << endl;
    //onStartTurn();
}
void Game::switchPlayer()
{
    curr_state.currPlayerReference++;
    curr_state.currPlayerReference %= NUM_PLAYERS;

    turnManager->setCurrentPlayer(getCurrentPlayer());
    turnManager->init();
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
            discardedTile = tile;
            continue;
        } else {
            successfullyTookTile = true;
            break;
        }
    }
    if (!successfullyTookTile) throw NoMoreTilesError(); return;
}
void Game::getDiscard(Tile *tile)
{
    discardedTile = tile;
    cout << "the tile " << tile->get_val() << " was discarded by player " << wind_strings[getCurrentPlayer()->_wind] << endl;
    // after termining no one wants the the discarded tile
}
void Game::finishGame()
{
    // check if any players have Mahjong, otherwise
    roundIsOver = true;
    playExtraHand = true;
    // update status to Draw
    cout << "finished Game" << endl;
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
            // update dealer
            curr_state.dealerReference++;
            curr_state.dealerReference %= NUM_PLAYERS;
            curr_state.currPlayerReference = curr_state.dealerReference;
            updatePlayerWinds();
        }
    } else {
        playExtraHand = false;
        curr_state.currPlayerReference = curr_state.dealerReference;
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
    delete turnManager;
    for (Tile *tile : pile) {
        delete tile;
    }
    for (int i = 0; i < NUM_PLAYERS; i++)
        delete curr_state.players[i];
}
