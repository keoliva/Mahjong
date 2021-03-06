/*
 * Mahjong
 *
 * Written by Kayla Oliva 2015
 *
 */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else 
#include "windows.h"
#include <GL/glut.h>
#endif
#include <algorithm>
#include <curl/curl.h>
#include <iostream>
#include <math.h>
#include <string>
#include <stdlib.h>
#include <vld.h>
#include "include/Game.h"
#include "include/HandEvaluator.h"
#include "include/HumanPlayer.h"
#include "include/Draw.h"

using namespace std;
static Draw *draw = nullptr;
static Game *game = nullptr;
static float rot_x = -86.0f, rot_y = 0.0f, rot_z = 0.0f;
int selectedTileIndex = 0;
bool mouseMoved = false;
/* GLUT callback Handlers */

static void resize(int width, int height) {
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
}

static void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearStencil(0);
    glMatrixMode(GL_MODELVIEW); // reset transformations
    glLoadIdentity();

    mouseActivity mouseInfo;
    mouseInfo = mouseActivity(mouseMoved, selectedTileIndex);

    draw->drawGame(rot_x, rot_y, rot_z, mouseInfo, game);

    selectedTileIndex = 0;
    mouseMoved = false;

    glutSwapBuffers();
}

static void idle() {
    HumanPlayer *human = dynamic_cast<HumanPlayer*>(game->getCurrentPlayer());
    HumanPlayer *humanPlayer = dynamic_cast<HumanPlayer*>(game->getPlayer(game->humanPlayerIndex));
    if ((game->getStatus() == In_Play(WAITING_FOR_INPUT_AFTER_DRAW) ||
         game->getStatus() == In_Play(WAITING_FOR_INPUT_TO_DISCARD) ||
         game->getStatus() == In_Play(WAITING_FOR_INPUT_AFTER_DECLARATION)) &&
        human && !human->hasProvidedInput()) {
       glutIdleFunc(NULL);
    } else if (game->getStatus() == In_Play(WAITING_FOR_INPUT_ON_DISCARD) &&
               !humanPlayer->hasProvidedInput()) {
        glutIdleFunc(NULL);
	} else if (game->roundOver()) {
		glutIdleFunc(NULL);
	}
	else {
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
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN || game->roundOver()) return;
    int window_height = glutGet(GLUT_WINDOW_HEIGHT);

    GLuint index;
    glReadPixels(x, window_height - y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);
    if (index > 0 && game->getStatus() == In_Play(WAITING_FOR_INPUT_AFTER_DECLARATION)) {
        Player *human = game->getCurrentPlayer();
        pair<Declaration, int> humanDeclaration = human->getDeclaration();
        humanDeclaration.second = index - 1;
        human->setDeclaration(humanDeclaration);
        glutIdleFunc(idle);
    } else if (index > 0 && game->getStatus() == In_Play(WAITING_FOR_INPUT_TO_DISCARD)) {
        game->getCurrentPlayer()->discardTile(index - 1);
        glutIdleFunc(idle);
    }
}
static void mouseMove(int x, int y) {
    if (game->roundOver() ||
        game->getStatus() != In_Play(WAITING_FOR_INPUT_TO_DISCARD)) {
        mouseMoved = false; return;
        }
    int window_height = glutGet(GLUT_WINDOW_HEIGHT);
    GLuint index;
    glReadPixels(x, window_height - y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);
    if (index > 0) {
        mouseMoved = true;
        selectedTileIndex = index;
        glutPostRedisplay();
    }
}

void cleanup();
static void key(unsigned char key, int x, int y) {
    switch (key) {
        case 27 :
        case 'q':
            exit(0);
            break;
		case 'r':
			if (game->roundOver()) {
				game->restart();
			}
			break;
        case 's':
            // status is only set when the current player is human, therefore current player
            // must be human
            if (game->getStatus() == In_Play(WAITING_FOR_INPUT_AFTER_DRAW)) {
                (game->getCurrentPlayer())->setDeclaration(
                                            make_pair(Declaration::NONE, -1));
                glutIdleFunc(idle);
            } else if (game->getStatus() == In_Play(WAITING_FOR_INPUT_ON_DISCARD)) {
                (game->getPlayer(game->humanPlayerIndex))->setDeclaration(
                                            make_pair(Declaration::NONE, -1));
                glutIdleFunc(idle);
            } else {
                glutIdleFunc(dealTiles);
            }
            break;
        case 'k':
            if (game->getStatus() == In_Play(WAITING_FOR_INPUT_AFTER_DRAW)) {
                (game->getCurrentPlayer())->setDeclaration(
                                        make_pair(Declaration::CONCEALED_KANG, -1));
                glutIdleFunc(idle);
            } else if (game->getStatus() == In_Play(WAITING_FOR_INPUT_ON_DISCARD)) {
                (game->getPlayer(game->humanPlayerIndex))->setDeclaration(
                                        make_pair(Declaration::MELDED_KANG, -1));
                glutIdleFunc(idle);
            }
            break;
        case 'm':
            if (game->getStatus() == In_Play(WAITING_FOR_INPUT_AFTER_DRAW)) {
                (game->getCurrentPlayer())->setDeclaration(
                                        make_pair(Declaration::SMALL_MELDED_KANG, -1));
                glutIdleFunc(idle);
			} else if (game->getStatus() == In_Play(WAITING_FOR_INPUT_ON_DISCARD)) {
				(game->getPlayer(game->humanPlayerIndex))->setDeclaration(
										make_pair(Declaration::MAHJONG, -1));

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
            if (game->getStatus() == In_Play(WAITING_FOR_INPUT_AFTER_DRAW)) {
                (game->getCurrentPlayer())->setDeclaration(
                                        make_pair(Declaration::SMALL_MELDED_KANG, -1));
                glutIdleFunc(idle);
            } else if (game->getStatus() == In_Play(WAITING_FOR_INPUT_ON_DISCARD)) {
                (game->getPlayer(game->humanPlayerIndex))->setDeclaration(
                                        make_pair(Declaration::MELDED_CHI, -1));
                glutIdleFunc(idle);
            }
            break;
        case 'p':
            if (game->getStatus() == In_Play(WAITING_FOR_INPUT_ON_DISCARD)) {
                (game->getPlayer(game->humanPlayerIndex))->setDeclaration(
                                        make_pair(Declaration::MELDED_PENG, -1));
                glutIdleFunc(idle);
            }
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
    game = new Game();
    draw = new Draw();
}
/* Program entry point */
void testInitGame() {
    HandEvaluator eval = HandEvaluator();
    eval.test();
}

void cleanup() {
	delete game;
	delete draw;
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

	//testInitGame();
	atexit(cleanup);
	glutMainLoop();
	return EXIT_SUCCESS;
}
