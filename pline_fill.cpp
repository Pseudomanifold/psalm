/*!
*	@file	pline_fill.cpp
*	@brief	Simple tool for filling polygonal holes in meshes
*	@author	Bastian Rieck <bastian.rieck@iwr.uni-heidelberg.de>
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <getopt.h>

#include "hole.h"

/*!
*	Processes a .pline file, in which each line consists of a hole,
*	described as a closed chain of vertices.
*
*	@param filename Filename
*/

void process_pline_file(std::string filename)
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
		std::cout << "pline_fill: Processing file \"" << filename << "\" " << std::flush;

	std::string line;
	std::stringstream converter;

	size_t curr_id = 0;	// stores ID of _current_ pline
	size_t prev_id = 0;	// stores ID of _previous_ pline
	size_t counter = 0;	// stores counter for group of plines. Since
				// several plines may share one ID, the counter
				// is required to store them in individual
				// files of the form "<Filename>_<ID>_<Counter>.ply".

	while(std::getline(in, line))
	{
		// Ignore lines containing a "#"
		if(line.find_first_of('#') != std::string::npos)
			continue;

		converter.str(line);

		// Data format is straightforward (different fields are assumed to
		// be separated by whitespace).
		//
		//	Label ID | number of vertices | x1 y1 z1 x2 y2 z2 ...

		size_t num_vertices;

		// Vector of vertices (actually, only the _positions_) for the
		// current polygonal line
		std::vector<v3ctor> vertices;

		converter >> curr_id >> num_vertices;
		if(curr_id == prev_id)
			counter++;
		else
			counter = 0; // reset counter if the next group of plines is reached

		for(size_t i = 0; i < num_vertices; i++)	// ignore last point because it is a repetition of
								// the first point
		{
			v3ctor v;
			converter >> v[0] >> v[1] >> v[2];

			// XXX: Ignore normals
			double dummy;
			converter >> dummy >> dummy >> dummy;

			vertices.push_back(v);
		}

		psalm::hole H;
		H.initialize(vertices);
		H.triangulate();

		// Construct filename
		converter.str("");
		converter.clear();
		converter	<< std::setw(4) << std::setfill('0')
				<< filename << "_"
				<< curr_id << "_"
				<< counter << ".ply";

		H.save(converter.str());

		converter.clear();
		line.clear();

		std::cout << "." << std::flush;
	}

	std::cout << "done." << std::endl;
}

int main(int argc, char* argv[])
{
	static option cmd_line_opts[] =
	{
		{"help",	no_argument,	NULL,	'h'},
		{NULL, 0, NULL, 0}
	};

	int option = 0;
	while((option = getopt_long(argc, argv, "h", cmd_line_opts, NULL)) != -1)
	{
		switch(option)
		{
			case 'h':
				break;
		}
	}

	// Further command-line parameters are assumed to be input files for
	// the program. These are handled sequentially.

	while(optind < argc)
		process_pline_file(argv[optind++]);

	return(0);
}
