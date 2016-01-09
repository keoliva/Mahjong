#include "include/Turn.h"
#include "include/AIPlayer.h"
#include "include/HumanPlayer.h"
#include <GL/glut.h>
#include <iostream>
int i = 0;

Turn::Turn()
{
    state_machine = StackFSM<State>();
}

Turn::~Turn()
{
    //delete state_machine;
}

void Turn::update()
{
    try {
        switch (state_machine.getCurrentState()) {
            case State::DISCARD_TILE:
                discardTile();
                break;
            case State::DRAW_TILE:
                drawTile();
                break;
            case State::SMALL_MELDED_KANG:
                break;
            case State::CONCEALED_KANG:
                break;
        }
    } catch (int e) {
        //game_instance->switchPlayer();
        state_machine.pushState(State::DRAW_TILE);
        drawTile();
    }
}

void Turn::drawTile()
{
    HumanPlayer *human = dynamic_cast<HumanPlayer*>(curr_player);
    PlayerStatus statuses[3] = {DECLARING_CONCEALED_KANG, DECLARING_SMALL_MELDED_KANG,
                                NOT_DECLARING_MELD};
    if (human && human->statusIn(statuses)) {
        state_machine.popState();
        if (human->statusIs(DECLARING_SMALL_MELDED_KANG)) {
            state_machine.pushState(State::DISCARD_TILE);
        } else if (human->statusIs(DECLARING_CONCEALED_KANG)) {
            state_machine.pushState(State::DISCARD_TILE);
        }  else if (human->statusIs(NOT_DECLARING_MELD)) {
            state_machine.pushState(State::DISCARD_TILE);
        }
        return;
    }
    std::cout << "Player " << wind_strings[curr_player->_wind] << "is drawing a tile from the wall" << std::endl;
    std::cout << "taking from a wall..." << i <<  std::endl;
    try {
        game_instance->takeFromWall(curr_player);
        std::cout << "COMPLETE!" << std::endl;
    } catch (NoMoreTilesError &e) {
        std::cout << "woops. no more tiles to grab..." << std::endl;
        game_instance->updateStatus(new Over(new GameDraw()));
        state_machine.popState();
        return;
    }
    curr_player->determineOptions();

    AIPlayer *ai;
    if (!human) {
        ai = dynamic_cast<AIPlayer*>(curr_player);
        Declaration declaration = ai->getDeclaration();
        if (declaration == Declaration::SMALL_MELDED_KANG) {
            state_machine.popState();
            state_machine.pushState(State::SMALL_MELDED_KANG);
            return;
        } else if (declaration == Declaration::CONCEALED_KANG) {
            state_machine.popState();
            state_machine.pushState(State::CONCEALED_KANG);
            return;
        }
    } else {
        std::map<MeldType, std::vector<meld>> options = human->getOptions();
        // see if human player CAN declare either one of these melds
        if (!options[CONCEALED_KANG].empty() || !options[SMALL_MELDED_KANG].empty()) {
            game_instance->updateStatus(new In_Play(WAITING_FOR_INPUT_AFTER_DRAW));
            return;
        }
    }
    state_machine.popState();
    state_machine.pushState(State::DISCARD_TILE);
}

void Turn::discardTile()
{
    game_instance->setDiscard(curr_player->discardTile());

    std::cout << "finished discarding tile " << i++ << std::endl;
    state_machine.popState();
    game_instance->cycleCurrentPlayer();
}
