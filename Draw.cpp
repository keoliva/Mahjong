#include "include/Draw.h"
#include "include/Obj.h"
#include <GL/gl.h>
#include <GL/glut.h>
#include <map>
#include <sstream>
#define BOARD_SIZE 20
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
int rows = BOARD_SIZE, cols = BOARD_SIZE;
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
    glEnd();
    glBegin(GL_LINES);
        for (float i=0; i < cols+1; i++) {
            glVertex3f(0, i, -tile_len/2);
            glVertex3f(rows*cell_width, i, -tile_len/2);
        }
    glEnd();
    glPopMatrix();
}
// displays a floating cone that serves as a pointer above the dealer's (east player's) hand
void drawEastPlayerMarker(PlayingOrder playerIndex)
{
    glColor3d(1, 0, 0);
    glPushMatrix();
    boardLoc loc;
    if (playerIndex == HUMAN) {
        loc = boardLoc(BOARD_SIZE/10, 0);
    } else if (playerIndex == RIGHT_OF_HUMAN) {
        loc = boardLoc(0, BOARD_SIZE/10);
    } else if (playerIndex == ACROSS_HUMAN) {
        loc.x = BOARD_SIZE - (BOARD_SIZE/10);
        loc.y = loc.x;
    } else if (playerIndex == LEFT_OF_HUMAN) {
        loc = boardLoc(0, BOARD_SIZE - (BOARD_SIZE/10));
    }
    glTranslated(loc.x, loc.y, tile_height*2);
    glRotated(180, 0, 1, 0);
    glutSolidCone(0.5, 1.0, 10, 10);
    glPopMatrix();
}
static float faceHuman = 0.0f, turnLeft = -90.0f, turnRight = 90.0f, faceAway = 180.0f;
static float standUp = 90.0f, lieDown = 0.0f;
boardLoc Draw::getPieceLoc(HandTile type, int ith, PlayingOrder playerIndex)
{
    boardLoc loc;
    if (type == REGULAR || type == REVEALED) {
        if (playerIndex == HUMAN) {
            loc = boardLoc(ith, (type == REGULAR)?0:2, standUp, faceHuman);
        } else if (playerIndex == RIGHT_OF_HUMAN) {
            loc = boardLoc(BOARD_SIZE-((type == REGULAR)?1:2), ith, standUp, turnRight);
        } else if (playerIndex == ACROSS_HUMAN) {
            loc = boardLoc((BOARD_SIZE - 1) - ith, BOARD_SIZE-((type == REGULAR)?1:2), standUp, faceAway);
        } else if (playerIndex == LEFT_OF_HUMAN) {
            loc = boardLoc((type == REGULAR)?0:2, (BOARD_SIZE - 1) - ith, standUp, turnLeft);
        }
        if (type == REGULAR) return loc;
    } else if (type == DISCARDED) {
        if (playerIndex == HUMAN) {
            //loc = boardLoc(,);
            loc.rotY = faceHuman;
        } else if (playerIndex == RIGHT_OF_HUMAN) {
            //loc = boardLoc(,);
            loc.rotY = turnRight;
        } else if (playerIndex == ACROSS_HUMAN) {
            //loc = boardLoc(,);
            loc.rotY = faceAway;
        } else if (playerIndex == LEFT_OF_HUMAN) {
            //loc = boardLoc(,);
            loc.rotY = turnLeft;
        }
    }
    // either REVEALED or DISCARDED at this point
    /** take the lying down piece of the human, and rotate the piece
      * around the z-axis to represent everyone else's */
    loc.rotX = lieDown;
    loc.z = -tile_height;
    loc.rotZ = loc.rotY;
    loc.rotY = faceHuman;

    return loc;
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

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    Player *player;
    PlayingOrder playerPos;
    int humanIndex = game->humanPlayerIndex;
    int handSize;
    int firstIndex, lastIndex;
    unsigned int pieceIndex=0;
    for (int i = 0; i < NUM_PLAYERS; i++) {
        player = game->getPlayer(i);
        playerPos = static_cast<PlayingOrder>(i);
        boardLoc loc;

        if (player->_wind == EAST) drawEastPlayerMarker(playerPos);

        // draw regular hand
        handSize = player->hand.size();
        firstIndex = (BOARD_SIZE - handSize)/2; lastIndex = firstIndex + handSize;
        {
            for (int j = firstIndex; j < lastIndex; j++) {
                if (i == humanIndex) glStencilFunc(GL_ALWAYS, pieceIndex + 1, -1);
                loc = getPieceLoc(REGULAR, j, playerPos);
                tile.draw(loc.x, loc.y, loc.z, loc.rotX, loc.rotY);
                pieceIndex++;
            }
        }
        if (i == humanIndex) {
            glDisable(GL_STENCIL_TEST); // only the hand pieces can be selected
            ss << " Round Wind: " << game->getPrevailingWind() << "   |   ";
            ss << " Your Wind: " << wind_strings[player->_wind] << "   |   ";
            ss << " Your Score: " << player->score;
            updates["userInfo"] = ss.str();
        }
        // draw revealed melds and bonus tiles
        lastIndex = firstIndex + player->bonuses.size() + player->melds.size();
        {
            Tile *revealed_tile;
            for (int j = firstIndex; j < lastIndex; j++) {
                if (pieceIndex < player->melds.size())
                    revealed_tile = player->melds[pieceIndex];
                else
                    revealed_tile = player->bonuses[pieceIndex];
                loc = getPieceLoc(REVEALED, j, playerPos);
                cout << "loc.z: " << loc.z << endl;
                tile.draw(loc.x, loc.y, loc.z, loc.rotX, loc.rotY, loc.rotZ);
                pieceIndex++;
            }
        }
        // draw discarded tiles
        /**{
            int j = 0;
            for (Tile *discarded_tile : player->discards) {
                loc = getPieceLoc(j, DISCARDED, playerPos);
                tile.draw(loc.x, loc.y, loc.z, loc.rotX, loc.rotY, loc.rotZ);
                pieceIndex++;
            }
        }*/
    }
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
