#ifndef DRAW_H
#define DRAW_H
#include "Game.h"

class Draw
{
    public:
        Draw();
        void drawGame(float rot_x, float rot_y, float rot_z, Game *game=nullptr);
};

#endif // DRAW_H
