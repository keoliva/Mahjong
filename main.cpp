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

#include <stdlib.h>
#include <iostream>
#include <string>
#include <math.h>
#include "include/Game.h"
#include "include/InputHandler.h"
#include "include/HandEvaluator.h"
#include "include/Obj.h"
#include "include/Draw.h"
#define BOOST_BIND_NO_PLACEHOLDERS
#include <boost/lambda/lambda.hpp>
using namespace std;

static Draw *draw;
static Game *game;
static float rot_x = -86.0f, rot_y = 0.0f, rot_z = 0.0f;
int selectedTileIndex;
bool mouseMoved, mouseClicked;
/* GLUT callback Handlers */

static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
}

static void display(void)
{
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // reset transformations
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    mouseKeyActivity mouseInfo;
    mouseInfo = mouseKeyActivity(mouseMoved, mouseClicked, selectedTileIndex);

    cout <<" drawing game " << endl;
    //game->update();
    draw->drawGame(rot_x, rot_y, rot_z, mouseInfo, game);

    mouseClicked = false;
    selectedTileIndex = 0;

    glutSwapBuffers();
}

static void idle()
{
    game->update();
    glutPostRedisplay();
}

static void dealTiles()
{
    if (game->finishedDealing()) {
        //glutIdleFunc(idle);
        glutIdleFunc(NULL);
    } else {
        game->dealTiles();
    }
    glutPostRedisplay();
}

static void mouseButton(int button, int state, int x, int y)
{
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN || game->roundOver()
        || game->getCurrentPlayer() != game->getPlayer(game->humanPlayerIndex)) return;
    int window_height = glutGet(GLUT_WINDOW_HEIGHT);

    GLuint index;
    glReadPixels(x, window_height - y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);
    mouseClicked = true;
    selectedTileIndex = index;

    mouseKeyActivity mouseInfo;
    mouseInfo = mouseKeyActivity(mouseMoved, mouseClicked, selectedTileIndex);

    InputHandler inputHandler = InputHandler(mouseInfo);
    Command *command = inputHandler.handleInput();
    command->execute(game->getCurrentPlayer());
    //glutPostRedisplay();
}
static void mouseMove(int x, int y)
{
    if (game->roundOver() ||
        game->getCurrentPlayer() != game->getPlayer(game->humanPlayerIndex)) return;
    int window_height = glutGet(GLUT_WINDOW_HEIGHT);

    GLuint index;
    glReadPixels(x, window_height - y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);

    mouseMoved = true;
    selectedTileIndex = index;
    glutPostRedisplay();
}

static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27 :
        case 'q':
            delete draw;
            delete game;
            exit(0);
            break;
        case 's':
            glutIdleFunc(dealTiles);
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
static void init(void)
{
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
void testInitGame()
{
    cout << "Prevailing Wind == EAST: " << ((game->getPrevailingWind() == "EAST")?"PASSED":"FAILED") << endl;
    cout << "ROUND_OVER == FALSE: " << ((game->roundOver() == false)?"PASSED":"FAILED") << endl;
    wind pwind = game->getCurrentPlayer()->_wind;
    cout << "Current Player's Wind: " << ((pwind == EAST)?"PASSED":"FAILED") << endl;
    HandEvaluator eval = HandEvaluator();
    eval.test();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);

    glutCreateWindow("Mahjong");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutMouseFunc(mouseButton);
    //glutIdleFunc(dealTiles);
    glutPassiveMotionFunc(mouseMove);
    init();
    testInitGame();
    glutMainLoop();
    //using namespace boost::network;
    //using namespace boost::network::http;


    return EXIT_SUCCESS;
}
