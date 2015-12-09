#include "include/Draw.h"
#include "include/Obj.h"
#include <GL/gl.h>
#include <GL/glut.h>
#include <map>
#include <sstream>
using namespace std;

namespace {
    static Obj tile;
}

Draw::Draw()
{
    tile = Obj("tile");
    tile.loadObj();
}

static float y_coord = -2.0f, z_coord = -13.0f;

int rows=20, cols=20;
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
// displays a floating cone that serves as a pointer above the dealer's (east player's) hand
void drawEastPlayerMarker()
{
    glColor3d(1, 0, 0);
    glPushMatrix();
    glTranslated(5, 0, tile_height*1.5);
    glRotated(180, 0, 1, 0);
    glutSolidCone(0.5, 1.0, 10, 10);
    glPopMatrix();
}

map<string, string> updates;
void updateText();
void Draw::drawGame(float rot_x, float rot_y, float rot_z, Game *game)
{
    glTranslatef(x_coord, y_coord, z_coord);
    glRotated(rot_x, 1, 0, 0);
    glRotated(rot_y, 0, 1, 0);
    glRotated(rot_z, 0, 0, 1);

    drawBoard();

    stringstream ss;
    ss << "Tiles Left: " << game->getTilesLeft();
    updates["tilesLeft"] = ss.str();
    ss.str("");

    Player *player;
    int humanIndex = game->humanPlayerIndex;
    for (int i = 0; i < NUM_PLAYERS; i++) {
        player = game->getPlayer(i);
        cout << "Player " << i << endl;
        cout << "====================" << endl;
        for (Tile *tile : player->hand) {
            cout << tile->get_val();
        }
        cout << endl;
        if (i == humanIndex) {
            ss << " Round Wind: " << game->getPrevailingWind() << "   |   ";
            ss << " Your Wind: " << wind_strings[player->_wind] << "   |   ";
            ss << " Your Score: " << player->score;
            updates["userInfo"] = ss.str();
        }
        if (player->_wind == EAST) {
            drawEastPlayerMarker();
        }
    }
    tile.draw(5, 0, 90, 0);
    updateText();
}
void *font = GLUT_BITMAP_9_BY_15;
/** got the following two functions from tutorial at:
http://www.lighthouse3d.com/tutorials/glut-tutorial/bitmap-fonts-and-orthogonal-projections/
*/
void setOrthographicProjection(int w, int h);
void restorePerspectiveProjection(void);
void renderString(float x, float y, void *font, const char *str, int len);
void renderUpdates(string msg, int window_width, float height)
{
    const unsigned char *c_str = (const unsigned char *)msg.c_str();
    int c_str_width = glutBitmapLength(font, c_str);
    renderString((window_width-c_str_width)/2.0, height, font, msg.data(), msg.size());
}
void updateText()
{
    // dimensions of current window
    int window_width = glutGet(GLUT_WINDOW_WIDTH);
    int window_height = glutGet(GLUT_WINDOW_HEIGHT);
    setOrthographicProjection(window_width, window_height);

    renderUpdates(updates["tilesLeft"], window_width, window_height*5/6);
    renderUpdates(updates["userInfo"], window_width, window_height/7);

    restorePerspectiveProjection();
}
void setOrthographicProjection(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}
void restorePerspectiveProjection(void)
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
void renderString(float x, float y, void *font, const char *str, int len)
{
    glRasterPos2f(x, y);
    for (int i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}
