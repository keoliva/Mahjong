/*
 * Mahjong
 *
 * Written by Kayla Oliva 2015
 *
 */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif
#include <iostream>
#include <math.h>
#include <string>
#include <stdlib.h>
#include <typeinfo>
#include "include/Game.h"
#include "include/HandEvaluator.h"
#include "include/HumanPlayer.h"
#include "include/Obj.h"
#include "include/Draw.h"
#define BOOST_BIND_NO_PLACEHOLDERS
#include <boost/lambda/lambda.hpp>
using namespace std;
Obj tile;
static Draw *draw;
static Game *game;
static float rot_x = -86.0f, rot_y = 0.0f, rot_z = 0.0f;
int selectedTileIndex;
bool mouseMoved;
/* GLUT callback Handlers */

static void resize(int width, int height) {
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
}

static void display(void) {
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); // reset transformations
    glLoadIdentity();

    mouseActivity mouseInfo;
    mouseInfo = mouseActivity(mouseMoved, selectedTileIndex);

    draw->drawGame(rot_x, rot_y, rot_z, mouseInfo, game);

    selectedTileIndex = 0;

    glutSwapBuffers();
}

static void idle() {
    if (*(game->getStatus()) == In_Play(WAITING_FOR_INPUT_AFTER_DRAW)) {
        glutIdleFunc(NULL);
    } else {
        game->update();
    }
    glutPostRedisplay();
}

static void dealTiles() {
    if (game->finishedDealing()) {
        glutIdleFunc(idle);
    } else {
        game->dealTiles();
    }
    glutPostRedisplay();
}


static void mouseButton(int button, int state, int x, int y) {
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN || game->roundOver()
        || *game->getStatus() != In_Play(WAITING_FOR_INPUT_TO_DISCARD)
        || *game->getStatus() != In_Play(WAITING_FOR_INPUT_AFTER_DECLARATION)) return;
    int window_height = glutGet(GLUT_WINDOW_HEIGHT);

    GLuint index;
    glReadPixels(x, window_height - y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);
    if (index > 0 && *game->getStatus() == In_Play(WAITING_FOR_INPUT_AFTER_DECLARATION)) {
        Player *human = game->getCurrentPlayer();
        pair<Declaration, int> humanDeclaration = human->getDeclaration();
        humanDeclaration.second = index - 1;
        human->setDeclaration(humanDeclaration);
        glutPostRedisplay();
    }
}
static void mouseMove(int x, int y) {
    if (game->roundOver() ||
        *game->getStatus() != In_Play(WAITING_FOR_INPUT_TO_DISCARD)) return;
    int window_height = glutGet(GLUT_WINDOW_HEIGHT);

    GLuint index;
    glReadPixels(x, window_height - y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);
    if (index > 0) {
        mouseMoved = true;
        selectedTileIndex = index;
        glutPostRedisplay();
    }
}

static void key(unsigned char key, int x, int y) {
    switch (key) {
        case 27 :
        case 'q':
            delete draw;
            delete game;
            exit(0);
            break;
        case 's':
            // status is only set when the current player is human, therefore current player
            // must be human
            if (*game->getStatus() == In_Play(WAITING_FOR_INPUT_AFTER_DRAW)) {
                (game->getCurrentPlayer())->setDeclaration(
                                            make_pair(Declaration::NONE, -1));
            } else {
                glutIdleFunc(dealTiles);
            }
            break;
        case 'k':
            if (*game->getStatus() == In_Play(WAITING_FOR_INPUT_AFTER_DRAW)) {
                (game->getCurrentPlayer())->setDeclaration(
                                        make_pair(Declaration::CONCEALED_KANG, -1));
            }
            break;
        case 'm':
            if (*game->getStatus() == In_Play(WAITING_FOR_INPUT_AFTER_DRAW)) {
                (game->getCurrentPlayer())->setDeclaration(
                                        make_pair(Declaration::SMALL_MELDED_KANG, -1));
            }
            break;
        case 'a':
            rot_x -= 1.0f;
            cout << "rot_x: " << rot_x << endl;
            break;
        case 'b':
            rot_x += 1.0f;
            cout << "rot_x: " << rot_x << endl;
            break;
        case 'c':
            rot_y -= 1.0f;
            break;
        case 'd':
            rot_y += 1.0f;
            break;
        case 'e':
            rot_z -= 1.0f;
            break;
        case 'f':
            rot_z += 1.0f;
            break;
    }
    glutPostRedisplay();
}

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat sec_light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat sec_light_diffuse[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
const GLfloat sec_light_position[] = { 2.0, 5.0f, 15.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.4f, 0.4f, 0.4f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };
static void init(void) {
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHT3);
    //glLightfv(GL_LIGHT3, GL_AMBIENT,  light_ambient);
    //glLightfv(GL_LIGHT3, GL_DIFFUSE,  sec_light_diffuse);
    //glLightfv(GL_LIGHT3, GL_SPECULAR, sec_light_specular);
    //glLightfv(GL_LIGHT3, GL_POSITION, sec_light_position);


    //glLightfv(GL_LIGHT0, GL_POSITION, sec_light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, high_shininess);

    draw = new Draw();
    game = new Game();
}
/* Program entry point */
void testInitGame() {
    HandEvaluator eval = HandEvaluator();
    eval.test();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);

    glutCreateWindow("Mahjong");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutMouseFunc(mouseButton);
    //glutIdleFunc(idle);
    glutPassiveMotionFunc(mouseMove);
    init();
    testInitGame();
    glutMainLoop();

    //using namespace boost::network;
    //using namespace boost::network::http;


    return EXIT_SUCCESS;
}
