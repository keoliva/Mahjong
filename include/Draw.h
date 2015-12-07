#ifndef DRAW_H
#define DRAW_H
#include "Obj.h"


class Draw
{
    public:
        Draw();
        virtual ~Draw();
        void drawGame(float rot_x, float rot_y, float rot_z);
    protected:
    private:
        Obj tile;
};

#endif // DRAW_H
