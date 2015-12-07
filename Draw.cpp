#include "include/Draw.h"
#include "include/Obj.h"

namespace {
    static Obj tile;
}

Draw::Draw()
{
    tile = Obj("tile");
    tile.loadObj();
}

static float y_coord = -2.0f, z_coord = -13.0f;

static int rows=20, cols=20;
static float tile_len = 0.968, tile_height = 1.489; // tile_width = tile_len
float space_0 = 0.5f, space_1 = 0.01f;
float x_coord = -cols/2 + space_0;
float cell_width = 1.0f, cell_height = 1.0f;

void drawBoard()
{
    glPushMatrix();
    glTranslatef(-cell_width/2, -tile_height/2+space_1, 0);
    glBegin(GL_LINES);
        for (float i=0; i < rows+1; i++) {
           glVertex3f(i, 0, -tile_len/2);
           glVertex3f(i, cols*cell_width, -tile_len/2);
        }
        for (float i=0; i < cols+1; i++) {
            glVertex3f(0, i, -tile_len/2);
            glVertex3f(rows*cell_width, i, -tile_len/2);
        }
    glEnd();
    glPopMatrix();
}

void Draw::drawGame(float rot_x, float rot_y, float rot_z)
{
    glTranslatef(x_coord, y_coord, z_coord);
    glRotated(rot_x, 1, 0, 0);
    glRotated(rot_y, 0, 1, 0);
    glRotated(rot_z, 0, 0, 1);

    drawBoard();


}
