/*!
*	@file	psalm.cpp
*	@brief	Main file for "psalm"
*
*	"psalm" (Pretty Subdivision ALgorithms on Meshes) is a CLI tool that
*	implements the Doo-Sabin and Catmull-Clark subdivision schemes.
*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <set>

#include <cerrno>
#include <cstring>

#include <unistd.h>
#include <getopt.h>
#include <libgen.h>

#include "mesh.h"

psalm::mesh scene_mesh;
std::string input;
std::string output;

/*!
*	Shows usage information for the program.
*/

void show_usage()
{
	std::cout	<< "psalm\n\n"
			<< "Usage: psalm [arguments] [file...]\n\n"
			<< "Arguments:\n\n"
			<< "-a, --algorithm <algorithm>\tSelect subdivision algorithm to use on the\n"
			<< "\t\t\t\tinput mesh. Valid values for <algorithm> are:\n\n"
			<< "\t\t\t\t\t* catmull-clark, catmull, clark, cc\n"
			<< "\t\t\t\t\t* doo-sabin, doo, sabin, ds\n"
			<< "\t\t\t\t\t* loop, l\n\n"
			<< "\t\t\t\tDefault algorithm: Catmull-Clark\n\n"
			<< "-c, --handle-creases\t\tSubdivide crease and boundary edges whenever\n"
			<< "\t\t\t\tthe algorithm supports this.\n\n"
			<< "-p, --parametric\t\tForces algorithms to compute new points using\n"
			<< "\t\t\t\tparametric methods and not geometric ones.\n\n"
			<< "-e, --extra-weights <file>\tOverride the default weights of subdivision\n"
			<< "\t\t\t\tschemes by reading them from <file>. The exact\n"
			<< "\t\t\t\tformat of this file depends on the subdivision\n"
			<< "\t\t\t\talgorithm that is used.\n\n"
			<< "-w, --weights <weights>\t\tSelect type of weights to that are used for\n"
			<< "\t\t\t\tthe subdivision scheme. Valid values are:\n\n"
			<< "\t\t\t\t\t[for the Doo-Sabin algorithm]\n"
			<< "\t\t\t\t\t* catmull-clark, catmull, clark, cc\n"
			<< "\t\t\t\t\t* doo-sabin, doo, sabin, ds\n\n"
			<< "\t\t\t\t\t[for all algorithms]\n"
			<< "\t\t\t\t\t* default\n\n"
			<< "-t, --type <type>\t\tSelect type of input data. Valid values for\n"
			<< "\t\t\t\tthe <type> parameter are:\n\n"
			<< "\t\t\t\t\t* ply (Standford PLY files)\n"
			<< "\t\t\t\t\t* obj (Wavefront OBJ files)\n"
			<< "\t\t\t\t\t* off (Geomview object files)\n\n"
			<< "-o, --output <file>\t\tSet output file\n\n"
			<< "-n, --steps <n>\t\t\tSet number of subdivision steps to perform on\n"
			<< "\t\t\t\tthe input mesh.\n\n"
			<< "\t\t\t\tDefault value: 0\n\n"
			<< "--remove-faces <numbers>\tRemove faces whose number of sides matches\n"
			<< "\t\t\t\tone of the numbers in the list. Use commas to\n"
			<< "\t\t\t\tseparate list values.\n\n"
			<< "--remove-vertices <numbers>\tRemove vertices whose valency matches one\n"
			<< "\t\t\t\tof the numbers in the list. Use commas to\n"
			<< "\t\t\t\tseparate list values.\n\n"
			<< "-s, --statistics\t\tPrint statistics to STDERR\n\n"
			<< "-h, --help\t\t\tShow this screen\n"
			<< "\n";
}

/*!
*	Reads a map of weights for a sudivision algorithm from a file. Each
*	line of the file must be of the following form:
*
*		<k> <a_1> ... <a_k>
*
*	Where <k> is the number of vertices for a face and <a_i> are the
*	weights that are used for the vertices of the face. The weights are
*	supposed to be arranged in counterclockwise order.
*
*	@param filename Filename of weights map
*	@return Associative array for quickly looking up the weights.
*/

psalm::weights_map load_weights_map(const std::string& filename)
{
	psalm::weights_map res;

	std::ifstream in;
	errno = 0;
	in.open(filename.c_str());

	if(!in.good() || errno)
	{
		std::string error = strerror(errno);
		std::cerr	<< "psalm: Could not load weights map file \""
				<< filename << "\": "
				<< error << "\n";

		return(res);
	}

	std::string line;
	std::istringstream converter;
	while(getline(in, line))
	{
		converter.clear();
		converter.str(line);

		std::vector<double> weights;
		size_t k = 0;

		// Read k and try to read k weights afterwards

		converter >> k;
		if(converter.fail())
		{
			std::cerr	<< "psalm: Unable to read number of weights "
					<< "from line \"" << line << "\"\n";
			return(res);
		}

		double w = 0.0;
		for(size_t i = 0; i < k; i++)
		{
			converter >> w;
			if(converter.fail())
			{
				std::cerr	<< "psalm: Unable to read weights "
						<< "from line \"" << line << "\"\n";
				return(res);
			}

			weights.push_back(w);
		}

		res[k] = weights;
	}

	return(res);
}

/*!
*	Handles user interaction.
*
*	@param argc Number of command-line arguments
*	@param argv Vector of command-line arguments
*/

int main(int argc, char* argv[])
{
	static option cmd_line_opts[] =
	{
		{"output",		required_argument,	NULL,	'o'},
		{"steps",		required_argument,	NULL,	'n'},
		{"type",		required_argument,	NULL,	't'},
		{"remove-faces",	required_argument,	NULL,	'F'},
		{"remove-vertices",	required_argument,	NULL,	'V'},
		{"algorithm",		required_argument,	NULL,	'a'},
		{"weights",		required_argument,	NULL,	'w'},
		{"extra-weights",	required_argument,	NULL,	'e'},

		{"statistics",		no_argument,		NULL,	's'},
		{"parametric",		no_argument,		NULL,	'p'},
		{"handle-creases",	no_argument,		NULL,	'c'},
		{"b-spline-weights",	no_argument,		NULL,	'b'},
		{"help",		no_argument,		NULL,	'h'},

		{NULL, 0, NULL, 0}
	};

	short type	= psalm::mesh::TYPE_EXT;
	short algorithm	= psalm::mesh::ALG_CATMULL_CLARK;
	short weights	= psalm::mesh::W_DEFAULT;

	std::set<size_t> remove_faces;
	std::set<size_t> remove_vertices;
	psalm::weights_map extra_weights;

	size_t steps	= 0;

	int option = 0;
	while((option = getopt_long(argc, argv, "o:n:F:V:t:a:w:e:bcph", cmd_line_opts, NULL)) != -1)
	{
		switch(option)
		{
			case 'o':
				output = optarg;
				break;

			case 't':
			{
				std::string type_str = optarg;
				std::transform(type_str.begin(), type_str.end(), type_str.begin(), (int(*)(int)) tolower);

				if(type_str == "ply")
					type = psalm::mesh::TYPE_PLY;
				else if(type_str == "obj")
					type = psalm::mesh::TYPE_OBJ;
				else if(type_str == "off")
					type = psalm::mesh::TYPE_OFF;
				else
				{
					std::cerr << "psalm: \"" << type_str << "\" is an unknown mesh data type.\n";
					return(-1);
				}

				break;
			}

			case 'a':
			{
				std::string algorithm_str = optarg;
				std::transform(algorithm_str.begin(), algorithm_str.end(), algorithm_str.begin(), (int(*)(int)) tolower);

				if(	algorithm_str == "catmull-clark"	||
					algorithm_str == "catmull"		||
					algorithm_str == "clark"		||
					algorithm_str == "cc")
					algorithm = psalm::mesh::ALG_CATMULL_CLARK;
				else if(algorithm_str == "doo-sabin"		||
					algorithm_str == "doo"			||
					algorithm_str == "sabin"		||
					algorithm_str == "ds")
					algorithm = psalm::mesh::ALG_DOO_SABIN;
				else if(algorithm_str == "loop"	||
					algorithm_str == "l")
					algorithm = psalm::mesh::ALG_LOOP;
				else
				{
					std::cerr << "psalm: \"" << algorithm_str << "\" is an unknown algorithm.\n";
					return(-1);
				}

				break;
			}

			case 'e':
			{
				extra_weights = load_weights_map(optarg);
				if(extra_weights.size() == 0)
				{
					std::cerr << "psalm: Unwilling to continue with empty weights file.\n";
					return(-1);
				}

				scene_mesh.set_custom_weights(extra_weights);
				break;
			}

			case 'w':
			{
				std::string weights_str = optarg;
				std::transform(weights_str.begin(), weights_str.end(), weights_str.begin(), (int(*)(int)) tolower);

				if(	weights_str == "catmull-clark"	||
					weights_str == "catmull"	||
					weights_str == "clark"		||
					weights_str == "cc")
					weights = psalm::mesh::W_CATMULL_CLARK;
				else if(weights_str == "doo-sabin"	||
					weights_str == "doo"		||
					weights_str == "sabin"		||
					weights_str == "ds")
					weights = psalm::mesh::W_DOO_SABIN;
				else if(weights_str == "degenerate")
					weights = psalm::mesh::W_DEGENERATE;
				else if(weights_str == "default")
					weights = psalm::mesh::W_DEFAULT;
				else
				{
					std::cerr << "psalm: \"" << weights_str << "\" is an unknown weight scheme.\n";
					return(-1);
				}

				scene_mesh.set_predefined_weights(weights);
				break;
			}

			case 'F':
			case 'I':
			{
				std::istringstream val_stream(optarg);
				std::istringstream converter;
				std::string val_str;
				while(getline(val_stream, val_str, ','))
				{
					converter.clear();
					converter.str(val_str);

					size_t value;
					converter >> value;
					if(converter.fail())
					{
						std::cerr << "psalm: Unable to convert \"" << val_str << "\" to a number.\n";
						return(-1);
					}

					if(option == 'F')
						remove_faces.insert(value);
					else
						remove_vertices.insert(value);
				}

				break;
			}

			case 'n':
			{
				std::istringstream converter(optarg);
				converter >> steps;
				if(converter.fail())
				{
					std::cerr << "psalm: Unable to  convert \"" << optarg << "\" to a number.\n";
					return(-1);
				}
				break;
			}

			case 'c':
				scene_mesh.set_crease_handling();
				break;

			case 'p':
				scene_mesh.set_parametric_point_creation();
				break;

			case 'b':
				scene_mesh.set_bspline_weights_usage();
				break;

			case 'h':
			case '?':
				show_usage();
				return(0);
		}
	}

	// Read further command-line parameters; these are all supposed to be
	// input files. If the user already specified an output file, only one
	// input file will be accepted.

	std::vector<std::string> files;
	while(optind < argc)
	{
		files.push_back(argv[optind++]);
		if(output.length() != 0 && files.size() > 1)
		{
			std::cerr << "psalm: Output file specified, but more than one input file present.\n";
			return(-1);
		}
	}

	// Replace the special file "-" by an empty string, thereby signalling
	// that standard input and standard output are to be used as file
	// streams.

	for(std::vector<std::string>::iterator it = files.begin(); it != files.end(); it++)
	{
		if(it->length() == 1 && (*it)[0] == '-')
			*it = "";
	}

	bool output_set = (output.length() > 0);
	if(output.length() == 1 && output[0] == '-')
		output = "";

	// Try to read from STDIN if no input files have been specified
	if(files.size() == 0)
		files.push_back("");

	// Apply subdivision algorithm to all files

	for(std::vector<std::string>::iterator it = files.begin(); it != files.end(); it++)
	{
		scene_mesh.load(*it, type);
		scene_mesh.subdivide(algorithm, steps);
		scene_mesh.prune(remove_faces, remove_vertices);

		// If an output file has been set (even if it is empty), it
		// will be used.
		if(output_set)
			scene_mesh.save(output, type);

		// If no output file has been set and the input file name is
		// not empty, the output will be written to a file.
		else if(it->length() > 0)
		{
			size_t ext_pos = (*it).find_last_of(".");
			if(ext_pos == std::string::npos)
				scene_mesh.save(*it+".subdivided", type);
			else
				scene_mesh.save( (*it).substr(0, ext_pos) + "_subdivided"
						+(*it).substr(ext_pos));
		}

		// If no output file has been set and the input file name is
		// empty, the output will be written to STDOUT.
		else
			scene_mesh.save("", type);
	}

	return(0);
}
