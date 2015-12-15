#include "Turn.h"
#include <GL/glut.h>
#include<iostream>
int i = 0;

void Turn::init()
{
    drawTile();
}

void Turn::drawTile()
{
    std::cout << "Player " << wind_strings[curr_player->_wind] << "is drawing a tile from the wall" << std::endl;
    std::cout << "taking from a wall..." << i <<  std::endl;
    try {
        game_instance->takeFromWall(curr_player);
        std::cout << "COMPLETE!" << std::endl;
        std::cout << "about to discard a tile..." << std::endl;
        glutPostRedisplay();
        discardTile();
    } catch (NoMoreTilesError &e) {
        std::cout << "woops. no more tiles to grab..." << std::endl;
        return game_instance->finishGame();
    }
}

void Turn::discardTile()
{
    game_instance->getDiscard(curr_player->discardTile());

    std::cout << "finished discarding tile " << i++ << std::endl;

    game_instance->switchPlayer();
    //glutPostRedisplay();
}
