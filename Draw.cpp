#include "include/Draw.h"
#include "include/HumanPlayer.h"
#include "include/Obj.h"
#include <GL/gl.h>
#include <GL/glut.h>
#include <cstring>
#include <ctype.h> // tolower
#include <map>
#include <sstream>
#define ROWS 22
#define COLS 22
#define BOARD_SIZE 20
using namespace std;

namespace {
    static Obj tile;
}
Draw::Draw() {
    mouseOverOther = false;
    tile = Obj("tile");
    tile.loadObj();
}

static float y_coord = -2.0f, z_coord = -14.0f;
static float tile_len = 0.968, tile_height = 1.489; // tile_width = tile_len
float space_0 = 0.5f, space_1 = 0.01f;
float x_coord = -COLS/2.0 + space_0;
float cell_width = 1.0f, cell_height = 1.0f;
void drawBoard() {
    glPushMatrix();
    glTranslatef(-cell_width/2, -tile_height/2+space_1, 0);
    glBegin(GL_LINES);
        for (float i=0; i < COLS+1; i++) {
           glVertex3f(i, 0, -tile_len/2);
           glVertex3f(i, ROWS*cell_width, -tile_len/2);
        }
    glEnd();
    glBegin(GL_LINES);
        for (float i=0; i < ROWS+1; i++) {
            glVertex3f(0, i, -tile_len/2);
            glVertex3f(COLS*cell_width, i, -tile_len/2);
        }
    glEnd();
    glPopMatrix();
}
// displays a floating cone that serves as a pointer above the dealer's (east player's) hand
void drawEastPlayerMarker(PlayingOrder playerIndex) {
    glColor3d(1, 0, 0);
    glPushMatrix();
    boardLoc loc;
    if (playerIndex == HUMAN) {
        loc = boardLoc(COLS/10, 0);
    } else if (playerIndex == RIGHT_OF_HUMAN) {
        loc = boardLoc(COLS-(COLS/10), ROWS/10);
    } else if (playerIndex == ACROSS_HUMAN) {
        loc.x = COLS - (COLS/10);
        loc.y = loc.x;
    } else if (playerIndex == LEFT_OF_HUMAN) {
        loc = boardLoc(0, ROWS - (ROWS/10));
    }
    glTranslated(loc.x, loc.y, tile_height*2);
    glRotated(180, 0, 1, 0);
    glutSolidCone(0.5, 1.0, 10, 10);
    glPopMatrix();
    glColor3d(1, 1, 1);
}
static float faceHuman = 0.0f, turnLeft = -90.0f, turnRight = 90.0f, faceAway = 180.0f;
static float standUp = 90.0f, lieDown = 0.0f, lieDownFaceDown = 180.0f;
boardLoc Draw::getPieceLoc(HandTile type, PlayingOrder playerIndex,
                           int ith, int row)
{
    boardLoc loc;
    if (playerIndex == HUMAN) {
        loc = boardLoc(ith, (type == REGULAR)?0:tile_height, standUp, faceHuman);
    } else if (playerIndex == RIGHT_OF_HUMAN) {
        loc = boardLoc(COLS-((type == REGULAR)?1:1+tile_height), ith, standUp, turnRight);
    } else if (playerIndex == ACROSS_HUMAN) {
        loc = boardLoc((COLS - 1) - ith, ROWS-((type == REGULAR)?1:1+tile_height), standUp, faceAway);
    } else if (playerIndex == LEFT_OF_HUMAN) {
        loc = boardLoc((type == REGULAR)?0:tile_height, (ROWS - 1) - ith, standUp, turnLeft);
    }
    if (type == REGULAR) return loc;
    float y;
    if (playerIndex == HUMAN || playerIndex == ACROSS_HUMAN)
        y = tile_height * (3 - row); // shift 4 units closer to the center of board
    else
        y = tile_height * (5 - row); // shift 4 units closer to the center of board
    if (type == DISCARDED) {
        if (playerIndex == HUMAN) {
            loc.y += y;
        } else if (playerIndex == RIGHT_OF_HUMAN) {
            loc.x -= y;
        } else if (playerIndex == ACROSS_HUMAN) {
            loc.y -= y;
        } else if (playerIndex == LEFT_OF_HUMAN) {
            loc.x += y;
        }
    }
    // either REVEALED, REVEALED_FACE_DOWN, or DISCARDED at this point
    // take the lying down piece of the human, and rotate the piece
    //  around the z-axis to represent everyone else's */
    loc.rotX = (type == REVEALED_FACE_DOWN)?lieDownFaceDown:lieDown;
    loc.z = -tile_height/4;
    loc.rotZ = loc.rotY;
    loc.rotY = faceHuman;

    return loc;
}

struct msg_data {
    string msg;
    float screen_height_ratio;
    msg_data() {};
    msg_data(string _msg, float y) : msg(_msg), screen_height_ratio(y) {};
};

map<string, msg_data> updates;
void updateText();
void Draw::drawGame(float rot_x, float rot_y, float rot_z, mouseActivity mouseInfo, Game *game)
{
    glTranslatef(x_coord, y_coord, z_coord);
    glRotated(rot_x, 1, 0, 0);
    glRotated(rot_y, 0, 1, 0);
    glRotated(rot_z, 0, 0, 1);
    drawBoard();

    updates.clear();
    stringstream ss;
    ss << "Tiles Left: " << game->getTilesLeft();
    updates["tilesLeft"] = msg_data(ss.str(), 5.0/6.0);
    ss.str("");
    bool drawBlinkingDiscard = false;
    if (game->getStatus() == In_Play(WAITING_FOR_INPUT_AFTER_DRAW) ||
        game->getStatus() == In_Play(WAITING_FOR_INPUT_ON_DISCARD)) {
        displayOptions(game->getPlayer(game->humanPlayerIndex));
        if (game->getStatus() == In_Play(WAITING_FOR_INPUT_ON_DISCARD)) {
            drawBlinkingDiscard = true;
        }
    } else if (game->getStatus() == In_Play(WAITING_FOR_INPUT_TO_DISCARD)) {
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    }

    Player *player, *curr_player = game->getCurrentPlayer();
    Tile *curr_discard = game->getDiscard();
    PlayingOrder playerPos;
    int humanIndex = game->humanPlayerIndex;
    int discardedRowLength;
    int firstIndex, lastIndex;
    unsigned int pieceIndex, handSize, meldsSize, discardsSize;
    string tilename;
    for (int i = 0; i < NUM_PLAYERS; i++) {
        player = game->getPlayer(i);
        playerPos = static_cast<PlayingOrder>(i);
        pieceIndex = 0;
        boardLoc loc;
        if (player->_wind == EAST) drawEastPlayerMarker(playerPos);
        // draw regular hand
        {
            handSize = player->hand.size();
            if (playerPos == HUMAN || playerPos == ACROSS_HUMAN)\
                firstIndex = (COLS - handSize)/2;
            else firstIndex = (ROWS - handSize)/2;
            lastIndex = firstIndex + handSize;
            for (int j = firstIndex; j < lastIndex; j++) {
                loc = getPieceLoc(REGULAR, playerPos, j);
                // if it's the human's turn, and this is the last tile, raise it
                HumanPlayer *human = dynamic_cast<HumanPlayer*>(game->getCurrentPlayer());
                if (game->getStatus() == In_Play(WAITING_FOR_INPUT_TO_DISCARD) && i == humanIndex) {
                    if (mouseInfo.mouseMoved && (mouseInfo.selectionIndex == pieceIndex + 1)) {
                        loc.z += tile_height/4; // raise the tile
                        mouseOverOther = true;
                    } else if (pieceIndex == handSize - 1 && !mouseOverOther ) {
                            loc.z += tile_height/4;
                    }
                    glStencilFunc(GL_ALWAYS, pieceIndex + 1, -1);
                } else if (game->getStatus() == In_Play() && i == humanIndex &&
                    human && human->getStatus() == PlayerStatus::DREW_TILE) {
                    if (pieceIndex == handSize - 1)
                        loc.z += tile_height/4;
                    mouseOverOther = false;
                }
                tilename = (i == humanIndex)?(player->hand[pieceIndex]->get_val()):"";
                tile.draw(loc.x, loc.y, loc.z, loc.rotX, loc.rotY, 0.0f, tilename);
                pieceIndex++;
            }
        }
        if (i == humanIndex) {
            glDisable(GL_STENCIL_TEST); // only the hand pieces can be selected
            ss << " Round Wind: " << game->getPrevailingWind() << "   |   ";
            ss << " Your Wind: " << wind_strings[player->_wind] << "   |   ";
            ss << " Your Score: " << player->score << " FAN";
            updates["userInfo"] = msg_data(ss.str(), 1.0/7.0);
        }
        // draw revealed melds and bonus tiles
        {
            meldsSize = player->melds.size();
            if (playerPos == HUMAN || playerPos == ACROSS_HUMAN)\
                firstIndex = (COLS - FULL_HAND_SIZE)/2;
            else firstIndex = (ROWS - FULL_HAND_SIZE)/2;
            lastIndex = firstIndex + player->bonuses.size() + meldsSize;
            int j = firstIndex;
            for (pair<MeldType, std::vector<Tile*>> meld : player->melds) {
                for (Tile *discard_tile : meld.second) {
                    if (meld.first == CONCEALED_KANG) {
                        loc = getPieceLoc(REVEALED_FACE_DOWN, playerPos, j);
                        tile.draw(loc.x, loc.y, loc.z, loc.rotX, loc.rotY, loc.rotZ, "");
                    } else {
                        loc = getPieceLoc(REVEALED, playerPos, j);
                        tile.draw(loc.x, loc.y, loc.z, loc.rotX, loc.rotY, loc.rotZ,
                              discard_tile->get_val());
                    }
                    j++;
                }
            }
            Tile *revealed_tile;
            pieceIndex = 0;
            for (; j < lastIndex; j++) {
                revealed_tile = player->bonuses[pieceIndex++];
                loc = getPieceLoc(REVEALED, playerPos, j);
                tile.draw(loc.x, loc.y, loc.z, loc.rotX, loc.rotY, loc.rotZ,
                              revealed_tile->get_val());
            }
        }
        // draw discarded tiles
        {
            if (playerPos == HUMAN || playerPos == ACROSS_HUMAN) {
                discardedRowLength = 11;
                firstIndex = (COLS - FULL_HAND_SIZE)/3;
            } else {
                discardedRowLength = 6;
                firstIndex = (ROWS - FULL_HAND_SIZE);
            }
            discardsSize = player->discards.size();
            int ith = firstIndex, row = 0;
            pieceIndex = 0;
            Tile *_tile;
            for (; pieceIndex < discardsSize; pieceIndex++) {
                loc = getPieceLoc(DISCARDED, playerPos, ith++, row);
                _tile = (player->discards)[pieceIndex];
                if (drawBlinkingDiscard &&
                    player == curr_player &&
                    Tile::areEqual(*_tile, *curr_discard)) {
                    tile.draw(loc.x, loc.y, loc.z, loc.rotX, loc.rotY, loc.rotZ,
                          _tile->get_val(), true);
                } else {
                    tile.draw(loc.x, loc.y, loc.z, loc.rotX, loc.rotY, loc.rotZ,
                          _tile->get_val());
                }
                if ((pieceIndex + 1) % discardedRowLength == 0) {
                        ith = firstIndex;
                        row++;
                }
            }

        }
    }
    if (game->getStatus() == In_Play(WAITING_FOR_INPUT_AFTER_DECLARATION)) {
        displayChoices(game->getPlayer(game->humanPlayerIndex), game->getDiscard());
    }
    updateText();
}

void determineLengths(float &x, float &y,
                      int num_sections, float total_len,
                      float _ratio=1.0f/3.0f) {
    // gives solution to system of equations
    // x * num_sections + y * (num_sections + 1) = total_len
    // y = x * _ratio
    if (x > 0) {
        y = (total_len - (x*num_sections + num_sections-1)) / 2;
        return;
    }
    x = total_len / (num_sections + _ratio*num_sections + _ratio);
    y = _ratio * x;
}
void Draw::displayChoices(Player *human, Tile *discardedTile) {
    map<MeldType, vector<meld>> options = human->getOptions();
    Declaration dec_type = human->getDeclaration().first;
    MeldType meld_type = Player::declarationToMeld[dec_type];
    vector<meld> possible_melds = options[meld_type];
    Tile *tile_in_meld;
    int num_tiles, choices_len = possible_melds.size(), j = 0;
    float x, y;
    float len = COLS*tile_len, z = tile_height*2;
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    for (meld _meld : possible_melds ) {
        switch (dec_type) {
            case Declaration::MELDED_PENG:
            case Declaration::SMALL_MELDED_KANG:
            case Declaration::CONCEALED_KANG:
            case Declaration::MELDED_KANG: {
                if (dec_type == Declaration::SMALL_MELDED_KANG) {
                    tile_in_meld = human->hand[_meld.indexInHand];
                } else {
                    tile_in_meld = human->hand[_meld.indicesInHand[0]];
                }
                num_tiles = (dec_type == Declaration::MELDED_PENG)?3:4;
                x = num_tiles;
                determineLengths(x, y, choices_len, len);
                glStencilFunc(GL_ALWAYS, j + 1, -1);
                glPushMatrix();
                glTranslatef(y + j*(num_tiles + 1), 0, 0);
                for (int i = 0; i < num_tiles; i++) {
                    boardLoc loc = getPieceLoc(REGULAR, HUMAN, i);
                    tile.draw(loc.x, loc.y, z, loc.rotX, loc.rotY, loc.rotZ,
                              tile_in_meld->get_val());
                }
                glPopMatrix();
                break;
            }
            case Declaration::MELDED_CHI:
                num_tiles = 3;
                x = num_tiles;
                determineLengths(x, y, choices_len, len);
                glStencilFunc(GL_ALWAYS, j + 1, -1);
                glPushMatrix();
                glTranslatef(y + j*(num_tiles + 1), 0, 0);
                for (int i = 0; i < num_tiles; i++) {
                    boardLoc loc = getPieceLoc(REGULAR, HUMAN, i);
                    if (_meld.indicesInHand[i] == -1) {
                        tile_in_meld = discardedTile;
                    } else {
                        tile_in_meld = human->hand[_meld.indicesInHand[i]];
                    }
                    tile.draw(loc.x, loc.y, z, loc.rotX, loc.rotY, loc.rotZ,
                              tile_in_meld->get_val());
                }
                glPopMatrix();
                break;
        }
        j++;
    }
    glDisable(GL_STENCIL_TEST);
    // draw transparent rectangle as tiles' background
    glPushMatrix();
        glTranslatef(0, 0, z);
        glRotated(standUp, 1, 0, 0);
        glColor4d(1, 1, 1, 0.3);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_POLYGON);
            glVertex2d(0, -tile_height); glVertex2d(len, -tile_height);
            glVertex2d(len, tile_height); glVertex2d(0, tile_height);
        glEnd();
        glDisable(GL_BLEND);
        glColor3d(1, 1, 1);
    glPopMatrix();
}
void Draw::displayOptions(Player *human) {
    map<MeldType, vector<meld>> options = human->getOptions();
    map<MeldType, vector<meld>>::iterator it;
    stringstream ss;
    ss << "Press 's' to skip.   ";
    string meld_type, key;
    vector<meld> possible_melds;
    for (it = options.begin(); it != options.end(); it++) {
        possible_melds = it->second;
        if (!possible_melds.empty()) {
            meld_type = meld_strings[it->first];
            if (meld_type == "SMALL_MELDED_KANG") {
                key = 'c';
            } else if (meld_type == "CONCEALED_KANG") {
                key = 'k';
            } else {
                const char *meld_type_c_str = meld_type.c_str();
                key = tolower(*(meld_type_c_str));
            }
            ss << "|   Press '" << key << "' to declare '" << meld_type << "'   ";
        }
    }
    updates["options"] = msg_data(ss.str(), 4.0/6.0);
}

void *font = GLUT_BITMAP_9_BY_15;
/** got the following two functions from tutorial at:
http://www.lighthouse3d.com/tutorials/glut-tutorial/bitmap-fonts-and-orthogonal-projections/
*/
void setOrthographicProjection(int w, int h);
void restorePerspectiveProjection(void);
void renderString(float x, float y, void *font, const char *str, int len);
void renderUpdates(string msg, int window_width, float height) {
    const unsigned char *c_str = (const unsigned char *)msg.c_str();
    int c_str_width = glutBitmapLength(font, c_str);
    renderString((window_width-c_str_width)/2.0, height, font, msg.data(), msg.size());
}
void updateText() {
    // dimensions of current window
    int window_width = glutGet(GLUT_WINDOW_WIDTH);
    int window_height = glutGet(GLUT_WINDOW_HEIGHT);
    setOrthographicProjection(window_width, window_height);

    map<string, msg_data>::iterator it;
    for (it = updates.begin(); it != updates.end(); it++) {
        renderUpdates((it->second).msg, window_width, window_height * (it->second).screen_height_ratio);
    }

    restorePerspectiveProjection();
}
void setOrthographicProjection(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}
void restorePerspectiveProjection(void) {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
void renderString(float x, float y, void *font, const char *str, int len) {
    glRasterPos2f(x, y);
    for (int i = 0; i < len; i++)
        glutBitmapCharacter(font, str[i]);
}
