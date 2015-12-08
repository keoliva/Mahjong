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
#include "include/Draw.h"
using namespace std;

static Draw *draw;
Game *game;
static float rot_x = -80.0f, rot_y = 0.0f, rot_z = 0.0f;
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

    draw->drawGame(rot_x, rot_y, rot_z);

    glutSwapBuffers();
}


static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27 :
        case 'q':
            //delete tile;
            delete draw;
            delete game;
            exit(0);
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

static void idle(void)
{
    glutPostRedisplay();
}

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };
static void init(void)
{
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, high_shininess);

    draw = new Draw();
    game = new Game();
}
/* Program entry point */
bool myfunction(Tile *a, Tile *b) { return a->get_val() < b->get_val(); }

void testInitGame()
{
    cout << "Prevailing Wind == EAST: " << ((game->getPrevailingWind() == "EAST")?"PASSED":"FAILED") << endl;
    cout << "ROUND_OVER == FALSE: " << ((game->roundOver() == false)?"PASSED":"FAILED") << endl;
    wind pwind = game->getCurrentPlayer()->_wind;
    cout << "Current Player's Wind: " << ((pwind == EAST)?"PASSED":"FAILED") << endl;
    cout << "Tiles created == 144: " << ((game->getTilesLeft() == 144)?"PASSED":"FAILED") << endl;
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
    //glutIdleFunc(idle);
    init();
    testInitGame();

    glutMainLoop();

    return EXIT_SUCCESS;
}
