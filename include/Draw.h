#ifndef DRAW_H
#define DRAW_H
#include "Game.h"

typedef struct boardLoc {
    int x, y;
    float z=0.0f, rotX=0.0f, rotY=0.0f, rotZ=0.0f;
    boardLoc() {};
    boardLoc(int _x, int _y) : x(_x), y(_y) {};
    boardLoc(int _x, int _y, float _rotX, float _rotY) : \
            x(_x), y(_y), rotX(_rotX), rotY(_rotY) {};
} boardLoc;
enum PlayingOrder {
    HUMAN, RIGHT_OF_HUMAN, ACROSS_HUMAN, LEFT_OF_HUMAN
};
enum HandTile {
    REGULAR, REVEALED, DISCARDED
};
class Draw
{
    public:
        Draw();
        boardLoc getPieceLoc(HandTile type, int ith, PlayingOrder playerIndex);
        void drawGame(float rot_x, float rot_y, float rot_z, Game *game=nullptr);
};

#endif // DRAW_H
