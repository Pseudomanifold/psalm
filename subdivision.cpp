/*!
*	@file	subdivision.cpp
*	@brief	Main file for demo program for Subdivision Surfaces
*/

#include <iostream>

#include <GL/glut.h>

#include "subdivision.h"
#include "mesh.h"
#include "zpr.h"

using namespace std;

mesh scene_mesh;
string input;
bool draw_lines = true;

void display_scene();
void init_scene();
void reshape_scene(int, int);
void keyboard_callback(unsigned char, int, int);

int main(int argc, char* argv[])
{
	// quick and dirty
	if(argc == 1)
	{
		cout << "Loading default mesh...\n";
		input = "";
	}
	else
	{
		cout << "Load user-specified mesh \"" << argv[1] << "\"\n";
		input = argv[1];
	}

	glutInit(&argc, argv);
	glutInitWindowPosition(WIN_X, WIN_Y);
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);

	glutInitDisplayMode(	GLUT_RGBA 	| 
				GLUT_DOUBLE 	|
				GLUT_DEPTH);
	
	glutCreateWindow(WIN_TITLE);

	init_scene();
	
	glutDisplayFunc(display_scene);
	glutReshapeFunc(reshape_scene);
	glutKeyboardFunc(keyboard_callback);

	glutPostRedisplay();
	glutMainLoop();
	return(0);
}

/*!
*	Initialization for the scene. Loads the mesh and sets some display
*	parameters.
*/

void init_scene(void)
{
	zprInit();

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_POLYGON_SMOOTH);

	glPointSize(5.0);

	scene_mesh.load(input.c_str());

	gluLookAt(	0.0, 1.0, 2.0,
		      	0.0, 0.0,-1.0,
			0.0, 1.0, 0.0);

}

/*!
*	Displays the scene, i.e. the current mesh and its subdivisions.
*/

void display_scene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
		scene_mesh.draw();
	glutSwapBuffers();
}

/*!
*
*	Reshape function for the scene. Maintains aspect ratio.
*
*	@param width  New width of window
*	@param height New height of window
*
*/

void reshape_scene(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, width/static_cast<GLfloat>(height), WIN_Z_NEAR, WIN_Z_FAR);
}

/*!
*	Keyboard callback function for program.
*
*	@param key	Key that has been pressed
*	@param x	x position of mouse cursor
*	@param y	y position of mouse cursor
*
*/

void keyboard_callback(unsigned char key, int x, int y)
{
	switch(tolower(key))
	{
		case 'e':
			scene_mesh.save("subdivided_"+input);
			break;

		case 'c':
			scene_mesh.subdivide_catmull_clark();
			break;

		case 'l':
			scene_mesh.subdivide_loop();
			break;

		case 'd':
			scene_mesh.subdivide_doo_sabin();
			break;

		case 'r':
			scene_mesh.load(input.c_str());
			break;

		case '\t':
			draw_lines = !draw_lines;
			break;

		case 'q':
			glutDestroyWindow(glutGetWindow());
			exit(0);
			break;

		default:
			break;
	}

	glutPostRedisplay();
}
