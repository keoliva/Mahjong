#include "Turn.h"
#include <GL/glut.h>
#include<iostream>
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
    switch (state_machine.getCurrentState()) {
        case NULL:
            game->switchPlayer();
            state_machine.pushState(State::DRAW_TILE);
            drawTile();
            break;
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
}

void Turn::drawTile()
{
    std::cout << "Player " << wind_strings[curr_player->_wind] << "is drawing a tile from the wall" << std::endl;
    std::cout << "taking from a wall..." << i <<  std::endl;
    try {
        game_instance->takeFromWall(curr_player);
        std::cout << "COMPLETE!" << std::endl;
        std::cout << "about to discard a tile..." << std::endl;
        state_machine.popState();
        state_machine.pushState(State::DISCARD_TILE);
    } catch (NoMoreTilesError &e) {
        std::cout << "woops. no more tiles to grab..." << std::endl;
        return game_instance->finishGame();
    }
    //glutPostRedisplay();
}

void Turn::discardTile()
{
    game_instance->getDiscard(curr_player->discardTile());

    std::cout << "finished discarding tile " << i++ << std::endl;
    state_machine.popState();
    game_instance->switchPlayer();
    //glutPostRedisplay();
}
