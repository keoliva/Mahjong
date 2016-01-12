#ifndef DRAW_H
#define DRAW_H
#include "Game.h"
#include "InputHandler.h"

typedef struct boardLoc {
    float x, y;
    float z=0.0f, rotX=0.0f, rotY=0.0f, rotZ=0.0f;
    boardLoc() {};
    boardLoc(float _x, float _y) : x(_x), y(_y) {};
    boardLoc(float _x, float _y, float _rotX, float _rotY) : \
            x(_x), y(_y), rotX(_rotX), rotY(_rotY) {};
} boardLoc;

enum PlayingOrder {
    HUMAN, RIGHT_OF_HUMAN, ACROSS_HUMAN, LEFT_OF_HUMAN
};
enum HandTile {
    REGULAR, REVEALED, REVEALED_FACE_DOWN, DISCARDED
};
class Draw {
    public:
        Draw();
        boardLoc getPieceLoc(HandTile type, PlayingOrder playerIndex, int ith, int row=0);
        void drawGame(float rot_x, float rot_y, float rot_z,
                      mouseKeyActivity mouseInfo, Game *game);
};

#endif // DRAW_H
