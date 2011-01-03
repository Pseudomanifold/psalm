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
#include <limits>

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
		//	Label ID | number of vertices | id1 x1 y1 z1 n1 n2 n3 id2 x2 y2 z2 ...

		size_t num_vertices;

		// Vector of vertices (actually, only the _positions_ and
		// vertex IDs) for the current polygonal line
		std::vector< std::pair<v3ctor, size_t> > vertices;

		converter >> curr_id >> num_vertices;
		if(curr_id == prev_id)
			counter++;
		else
			counter = 0; // reset counter if the next group of plines is reached

		for(size_t i = 0; i < num_vertices-1; i++)	// ignore last point because it is a repetition of
								// the first point
		{
			std::pair<v3ctor, size_t> vertex_w_id;
			vertex_w_id.second = std::numeric_limits<size_t>::max();

			converter >> vertex_w_id.second >> vertex_w_id.first[0] >> vertex_w_id.first[1] >> vertex_w_id.first[2];
			if(vertex_w_id.second == std::numeric_limits<size_t>::max())
			{
				std::cerr << "pline_fill: Could not parse .pline file -- missing a vertex ID\n";
				return;
			}

			// XXX: Ignore normals
			double dummy;
			converter >> dummy >> dummy >> dummy;

			vertices.push_back(vertex_w_id);
		}

		psalm::hole H;
		H.initialize(vertices);
		H.triangulate();
		try
		{
			H.subdivide(psalm::mesh::ALG_LIEPA);

			// Construct filename
			converter.str("");
			converter.clear();
			converter	<< filename << "_"
					<< std::setw(4) << std::setfill('0')
					<< curr_id << "_"
					<< std::setw(4) << std::setfill('0')
					<< counter << ".hole";

			H.save(converter.str());
		}
		catch(...)
		{
			std::cerr << "pline_fill: Unable to subdivide input data [" << curr_id << "," << counter << "]\n";
		}

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
