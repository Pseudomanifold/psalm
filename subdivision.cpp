/*!
*	@file	subdivision.cpp
*	@brief	Main file for demo program for Subdivision Surfaces
*/

#include <iostream>
#include <sstream>

#include <GL/glut.h>
#include <unistd.h>
#include <getopt.h>

#include "subdivision.h"
#include "mesh.h"
#include "zpr.h"

using namespace std;

mesh scene_mesh;
string input;
string output;

bool draw_lines = true;

void display_scene();
void init_scene();
void reshape_scene(int, int);
void keyboard_callback(unsigned char, int, int);

int main(int argc, char* argv[])
{
	static option cmd_line_opts[] =
	{
		{"output",	required_argument,	NULL,	'o'},
		{"steps",	required_argument,	NULL,	'n'},
		{"type",	required_argument,	NULL,	't'},

		{"help",	no_argument,		NULL,	'h'},

		{NULL, 0, NULL, 0}
	};

	short type	= mesh::TYPE_EXT;
	short algorithm	= mesh::ALG_CATMULL_CLARK; // FIXME: Needs to be chosen.

	size_t steps	= 1;

	int option = 0;
	while((option = getopt_long(argc, argv, "o:n:t:h", cmd_line_opts, NULL)) != -1)
	{
		switch(option)
		{
			case 'o':
				output = optarg;
				break;

			case 't':
			{
				string type_str = optarg;
				transform(type_str.begin(), type_str.end(), type_str.begin(), (int(*)(int)) tolower);

				if(type_str == "ply")
					type = mesh::TYPE_PLY;
				else if(type_str == "obj")
					type = mesh::TYPE_OBJ;
				else if(type_str == "off")
					type = mesh::TYPE_OFF;
				else
					cerr << "WTF? UNKNOWN TYPE.\n"; // FIXME

				break;
			}

			case 'n':
			{
				istringstream converter(optarg);
				converter >> steps;
				if(converter.fail())
				{
					// FIXME
					cerr << "CONVERTER ERROR\n";
				}
				break;
			}

			case 'h':
			case '?':
				// NYI
				break;
		}
	}

	// Read further command-line parameters; these are all supposed to be
	// input files. If the user already specified an output file, only one
	// input file will be accepted.

	vector<string> files;
	while(optind < argc)
	{
		files.push_back(argv[optind++]);
		if(output.length() != 0 && files.size() > 1)
		{
			cerr << "WTF?\n"; // FIXME
			return(0);
		}
	}

	// Replace the special file "-" by an empty string, thereby signalling
	// that standard input and standard output are to be used as file
	// streams.

	for(vector<string>::iterator it = files.begin(); it != files.end(); it++)
	{
		if(it->length() == 1 && (*it)[0] == '-')
			*it = "";
	}

	bool output_set = (output.length() > 0);
	if(output.length() == 1 && output[0] == '-')
		output = "";

	// Apply subdivision algorithm to all files

	for(vector<string>::iterator it = files.begin(); it != files.end(); it++)
	{
		scene_mesh.load(*it, type);
		scene_mesh.subdivide(algorithm, steps);

		// If an output file has been set (even if it is empty), it
		// will be used.
		if(output_set)
			scene_mesh.save(output, type);

		// If no output file has been set and the input file name is
		// not empty, the output will be written to a file.
		else if(it->length() > 0)
		{
			// FIXME: File name can be determined automatically
			scene_mesh.save(*it+".subdivided", type);
		}

		// If no output file has been set and the input file name is
		// empty, the output will be written to STDOUT.
		else
			scene_mesh.save("", type);
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
			scene_mesh.subdivide(mesh::ALG_CATMULL_CLARK);
			break;

		case 'l':
			scene_mesh.subdivide(mesh::ALG_LOOP);
			break;

		case 'd':
			scene_mesh.subdivide(mesh::ALG_DOO_SABIN);
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
