#include "include/Game.h"
#include "include/AIPlayer.h"
#include "include/HumanPlayer.h"
#include <GL/glut.h>
#include <cassert>
#include <chrono>
#include <random>
#include <typeinfo>
using namespace std;
int Game::humanPlayerIndex = 0;
Game::Game()
{
    roundIsOver = false;
    playExtraHand = false;
    rounds = 0;
	curr_discard = new NullTile();
    curr_status = In_Play();
    chooseDealer();
    curr_state.players[humanPlayerIndex] = new HumanPlayer();
    int i = (humanPlayerIndex + 1) % NUM_PLAYERS;
    for (; i != humanPlayerIndex; i = (i+1)%NUM_PLAYERS)
        curr_state.players[i] = new AIPlayer();
    updatePlayerWinds();

    turnManager = new Turn(this, getCurrentPlayer());
    tileDealer = new TileDealer(this);
    Tile::createTheTiles(wall);
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
    bool successfullyTookTile = false;
    while (!wall.empty()) {
		Tile *lastTile = wall.back().get();
         cout << "Tile being drawn: " << lastTile->get_val() << endl;
		player->takeTile(std::move(wall.back()));
		wall.pop_back();
        if (lastTile->type == BONUS) {
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
			clearPlayersHands();
        } else {
            // update round wind
            rounds = (rounds + 1) % NUM_ROUNDS;
            cycleDealer();
            updatePlayerWinds();
			clearPlayersHands();
        }
    } else {
        playExtraHand = false;
        curr_state.currPlayerReference = curr_state.dealerReference;
    }
    roundIsOver = false;
	curr_status = In_Play();
	//turnManager = new Turn(self, getCurrentPlayer());
    Tile::createTheTiles(wall);
}
void Game::updateStatus(Status new_status) {
	curr_status = new_status;
	if (typeid(curr_status).name() == "Over") {
		roundIsOver = true;
		Status *status = &curr_status;
		Over *over_status = dynamic_cast<Over*>(status);
		assert(over_status);
		Outcome *outcome = over_status->outcome;
		if (typeid(*outcome).name() == "Winner") {
			Winner *winner = dynamic_cast<Winner*>(outcome);
			assert(winner);
			// if the dealer won, the seats are maintained
			if (winner->playerWind == EAST) {
				playExtraHand = true;
			}
		} else { // game's a draw. everyone keeps their seats
			playExtraHand = true;
		}
	}
}
void Game::clearPlayersHands()
{
	Player *player;
	for (int i = 0; i < NUM_PLAYERS; i++) {
		player = curr_state.players[i];
		player->hand.clear();
		player->melds.clear();
		player->bonuses.clear();
		player->discards.clear();
	}
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
    return wall.size();
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
	delete curr_discard;
    delete turnManager;
    delete tileDealer;
    wall.clear();
    for (int i = 0; i < NUM_PLAYERS; i++)
        delete curr_state.players[i];
}
