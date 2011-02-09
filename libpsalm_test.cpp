/*!
*	@file	libpsalm_test.cpp
*	@brief	Test suite for libpsalm
*	@author	Bastian Rieck <bastian.rieck@iwr.uni-heidelberg.de>
*
*	This test suite reads a .pline file, converts it to a format suitable
*	for libpsalm, and writes the output file back into native PLY format.
*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include "libpsalm.h"

/*!
*	Processes the given .pline file by reading it and converting it to a
*	format suitable for libpsalm.
*
*	@param filename		File to process
*	@param ignore_IDs	If this flag is set, the original vertex IDs
*				will be ignored. This allows the filled hole to
*				be treated as a normal mesh.
*/

void process_pline_file(std::string filename, bool ignore_IDs = false)
{
	std::ifstream in(filename.c_str());
	if(!in.good())
	{
		std::cerr	<< "pline_fill: Unable to open .pline file \""
				<< filename << "\"."
				<< std::endl;
		return;
	}
	else
		std::cerr << "pline_fill: Processing file \"" << filename << "\" " << std::flush;

	std::string line;
	std::stringstream converter;

	size_t num_closed_holes = 0;
	while(std::getline(in, line))
	{
		// Ignore lines containing a "#"
		if(line.find_first_of('#') != std::string::npos)
			continue;

		converter.str(line);

		// Data format is straightforward (different fields are assumed to
		// be separated by whitespace).
		//
		//	Label ID | number of vertices | id1 x1 y1 z1 n1 n2 n3 id2 x2 y2 z2 ...

		int label_no;
		int num_vertices;

		converter >> label_no >> num_vertices;
		num_vertices--;	// ignore last point because it is a repetition
				// of the first point

		// Prepare storage for vertex IDs and vertex coordinates
		long* vertex_IDs = new long[num_vertices];
		double* coordinates = new double[3*num_vertices];
		double* normals = new double[3*num_vertices];

		for(int i = 0; i < num_vertices; i++)
		{
			converter	>> vertex_IDs[i]
					>> coordinates[3*i]
					>> coordinates[3*i+1]
					>> coordinates[3*i+2]
					>> normals[3*i]
					>> normals[3*i+1]
					>> normals[3*i+2];

			vertex_IDs[i]++; // ID == 0 would cause problems
		}

		// Storage for data returned by the algorithm
		int num_new_vertices	= 0;
		double* new_coordinates = NULL;
		int num_new_faces	= 0;
		long* new_vertex_IDs	= NULL;

		// These are thrown away _after_ reading. This could be solved
		// better.
		if(ignore_IDs)
		{
			delete[] vertex_IDs;
			vertex_IDs = NULL;
		}

		bool res = fill_hole(	num_vertices, vertex_IDs, coordinates, NULL, normals,
					&num_new_vertices, &new_coordinates, &num_new_faces, &new_vertex_IDs);


		if(res)
			num_closed_holes++;

		delete[] vertex_IDs;
		delete[] coordinates;
		delete[] normals;

		delete[] new_coordinates;
		delete[] new_vertex_IDs;

		converter.clear();
		line.clear();

		std::cerr << "." << std::flush;
	}

	std::cerr	<< std::endl
			<< "Closed " << num_closed_holes << " holes." << std::endl;
}

int main(int argc, char* argv[])
{
	//process_pline_file("../Holes/Salmanassar3_Holes_w_Normals_HR_CLEAN_with_Indices.pline", true);
	if(argc == 1)
		process_pline_file("../Holes/0976.pline", true);
	else
		process_pline_file(argv[1], true);
	return(0);
}
