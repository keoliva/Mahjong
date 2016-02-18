#ifndef DRAW_H
#define DRAW_H
#include "Game.h"

struct mouseActivity {
    bool mouseMoved;
    int selectionIndex;
    mouseActivity() : selectionIndex(0) {};
    mouseActivity(bool moved, int index) :\
        mouseMoved(moved), selectionIndex(index) {};
};

struct boardLoc {
    float x, y;
    float z=0.0f, rotX=0.0f, rotY=0.0f, rotZ=0.0f;
    boardLoc() {};
    boardLoc(float _x, float _y) : x(_x), y(_y) {};
    boardLoc(float _x, float _y, float _rotX, float _rotY) : \
            x(_x), y(_y), rotX(_rotX), rotY(_rotY) {};
};

enum PlayingOrder {
    HUMAN, RIGHT_OF_HUMAN, ACROSS_HUMAN, LEFT_OF_HUMAN
};
enum HandTile {
    REGULAR, REVEALED, REVEALED_FACE_DOWN, DISCARDED
};
class Draw {
    public:
        Draw();
        void drawGame(float rot_x, float rot_y, float rot_z,
                      mouseActivity mouseInfo, Game *game);
    private:
        bool mouseOverOther;
        void displayChoices(Player *player, Tile *discardedTile=nullptr);
        void displayOptions(Player *human);
		void drawGameOver(Status game_status);
        boardLoc getPieceLoc(HandTile type, PlayingOrder playerIndex, int ith, int row=0);
};

#endif // DRAW_H
