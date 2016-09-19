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

#include <boost/program_options.hpp>

#include <cerrno>
#include <cstring>

#include <unistd.h>
#include <getopt.h>
#include <libgen.h>

#include "SubdivisionAlgorithms/CatmullClark.h"
#include "SubdivisionAlgorithms/DooSabin.h"
#include "SubdivisionAlgorithms/Loop.h"
#include "SubdivisionAlgorithms/Liepa.h"

#include "FairingAlgorithms/FairingAlgorithm.h"

// FIXME: This should be conditional
#if 0
  #include "FairingAlgorithms/CurvatureFlow.h"
#endif

#include "mesh.h"

psalm::mesh scene_mesh;
std::string input;
std::string output;

namespace po = boost::program_options;

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
*	Parses a string of comma-separated numbers.
*
*	@param	argument Argument string
*	@return Set of numbers.
*/

std::set<size_t> parse_value_string(std::string argument)
{
	std::set<size_t> res;

	std::istringstream val_stream(argument);
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
			return(res);
		}

		res.insert(value);
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
	psalm::mesh::file_type type = psalm::mesh::TYPE_EXT;

	std::set<size_t> remove_faces;
	std::set<size_t> remove_vertices;
	psalm::weights_map extra_weights;

	size_t steps	= 0;

	psalm::SubdivisionAlgorithm* subdivision_algorithm	= NULL;
	psalm::FairingAlgorithm* fairing_algorithm		= NULL;

	// Add general program options

	po::options_description general("General", 80);
	general.add_options()
		(	"algorithm,a",
			po::value<std::string>(),
			"Selects subdivision algorithm to use on the input mesh. Valid values:\n"\
			"* catmull-clark, catmull, clark, cc [default]\n"\
			"* doo-sabin, doo, sabin, ds\n"\
			"* loop, l")

		(	"type,t",
			po::value<std::string>(),
			"Selects type of input data. Valid values:\n"\
			"* ply (Stanford PLY files)\n"\
			"* obj (Wavefront OBJ files)\n"\
			"* off (Geomview object files)")

		(	"output,o",
			po::value<std::string>(&output)->default_value(""),
			"Sets output file")

		(	"steps,n",
			po::value<size_t>(&steps),
			"Sets number of subdivision steps to perform on the input mesh.")

		(	"statistics,s",
			"Prints statistics to STDERR")

		(	"help,h",
			"Shows this screen");

	// Add tuning program options

	po::options_description tuning("Algorithm tuning", 80);
	tuning.add_options()
		(	"preserve-boundaries,p",
			"Forces subdivision algorithms to preserve the original boundaries of a mesh. "\
			"This is especially useful for open meshes. This flag is not taken into "\
			"account by every subdivision algorithm.")

		(	"handle-creases,c",
			"Subdivides crease and boundary edges whenever the algorithm supports this. "\
			"For the Catmull-Clark scheme, for example, midpoints of edges are used for "\
			"the new control points")

		(	"geometric,g",
			"Forces algorithms to compute new points using "\
			"geometric methods and forbids the use of parametric (i.e. weight-based) ones.")

		(	"b-spline-weights,b",
			"Forces algorithms to use the B-spline weights in regular cases, thereby "\
			"overriding any other weights scheme.")

		(	"extra-weights,e",
			po::value<std::string>(),
			"Overrides default weights of subdivision schemes by reading them from <arg>. "\
			"The exact format of this file depends on the subdivision algorithm that is used.")

		(	"weights,w",
			po::value<std::string>(),
			"Selects type of weights that are used for the subdivision scheme. Algorithms "\
			"may ignore unknown values. Valid values:\n"\
			"* catmull-clark, catmull, clark, cc\n"\
			"* default\n"\
			"* degenerate\n"\
			"* doo-sabin, doo, sabin, ds\n")

		(	"fair,f",
			"Performs a fairing step after working with the mesh.");

	// Add pruning program options

	po::options_description pruning("Pruning", 80);
	pruning.add_options()
		(	"remove-faces",
			po::value<std::string>(),
			"Remove faces whose number of sides matches one of the numbers in the list. "\
			"Use commas to separate list values.")

		(	"remove-vertices",
			po::value<std::string>(),
			"Remove vertices whose valency matches one of the numbers in the list. "\
			"Use commas to separate list values.");

	// Add hidden program options that are used for multiple input files

	po::options_description hidden_options("");
	hidden_options.add_options()
		(	"input",
			po::value< std::vector<std::string> >());

	// Describe visible options of the program and parse the command-line
	// using _all_ available options

	po::options_description visible_options("Usage: psalm [arguments] [files]");
	po::options_description all_options("");

	visible_options.add(general);
	visible_options.add(tuning);
	visible_options.add(pruning);

	all_options.add(visible_options);
	all_options.add(hidden_options);

	po::positional_options_description p;
	p.add("input", -1);

	po::variables_map vm;
	try
	{
		po::store(po::command_line_parser(argc, argv)	.options(all_options)
								.positional(p)
								.run(), vm);
		po::notify(vm);
	}
	catch(const boost::program_options::error& e)
	{
		std::cerr << "psalm: Options error: " << e.what() << "\n";
		return(-1);
	}

	// Actual handling of command-line options; mostly, parameters are set

	if(vm.count("help"))
	{
		std::cerr << all_options << "\n";
		return(0);
	}

	if(vm.count("type"))
	{
		std::string type_str = vm["type"].as<std::string>();
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
	}

	// As there is currently only _one_ fairing algorithm, there is really
	// not much choice here
	if(vm.count("fair"))
        {
// FIXME: Should be conditionally disabled...
#if 0
		fairing_algorithm = new psalm::CurvatureFlow();
#endif
        }

	// We use this instance to create an instance of a subdivision
	// algorithm class. Further class parameters are set _afterwards_,
	// sometimes depending on the type of subdivision algorithm.
	if(vm.count("algorithm"))
	{
		std::string algorithm_str = vm["algorithm"].as<std::string>();
		std::transform(algorithm_str.begin(), algorithm_str.end(), algorithm_str.begin(), (int(*)(int)) tolower);

		if(	algorithm_str == "catmull-clark"	||
			algorithm_str == "catmull"		||
			algorithm_str == "clark"		||
			algorithm_str == "cc")
		{
			subdivision_algorithm = new psalm::CatmullClark();
		}
		else if(algorithm_str == "doo-sabin"		||
			algorithm_str == "doo"			||
			algorithm_str == "sabin"		||
			algorithm_str == "ds")
		{
			subdivision_algorithm = new psalm::DooSabin();
		}
		else if(algorithm_str == "loop"	||
			algorithm_str == "l")
		{
			subdivision_algorithm = new psalm::Loop();
		}
		else if(algorithm_str == "liepa")
		{
			subdivision_algorithm = new psalm::Liepa();
		}
		else
		{
			std::cerr << "psalm: \"" << algorithm_str << "\" is an unknown algorithm.\n";
			return(-1);
		}
	}

	// Only applicable if the subdivision algorithm is the Doo-Sabin
	// subdivision scheme
	if(vm.count("extra-weights"))
	{
		psalm::DooSabin* ds_algorithm = dynamic_cast<psalm::DooSabin*>(subdivision_algorithm);
		if(ds_algorithm)
		{
			extra_weights = load_weights_map(vm["extra-weights"].as<std::string>());
			if(extra_weights.size() == 0)
			{
				std::cerr << "psalm: Unwilling to continue with empty weights file.\n";
				return(-1);
			}

			ds_algorithm->set_custom_weights(extra_weights);
		}
		else
			std::cerr << "psalm: Warning: Weights file specified, but no Doo-Sabin algorithm.\n";
	}

	if(vm.count("weights"))
	{
		std::string weights_str = vm["weights"].as<std::string>();
		std::transform(weights_str.begin(), weights_str.end(), weights_str.begin(), (int(*)(int)) tolower);

		if(	weights_str == "catmull-clark"	||
			weights_str == "catmull"	||
			weights_str == "clark"		||
			weights_str == "cc")
		{
			subdivision_algorithm->set_weights(psalm::SubdivisionAlgorithm::catmull_clark);
		}
		else if(weights_str == "doo-sabin"	||
			weights_str == "doo"		||
			weights_str == "sabin"		||
			weights_str == "ds")
		{
			subdivision_algorithm->set_weights(psalm::SubdivisionAlgorithm::doo_sabin);
		}
		else if(weights_str == "degenerate")
		{
			subdivision_algorithm->set_weights(psalm::SubdivisionAlgorithm::degenerate);
		}
		else
		{
			std::cerr << "psalm: \"" << weights_str << "\" is an unknown weight scheme.\n";
			return(-1);
		}
	}

	// This is parsed using an external function because the parameter
	// string consists of comma-separated values

	if(vm.count("remove-faces"))
		remove_faces = parse_value_string(vm["remove-faces"].as<std::string>());

	if(vm.count("remove-vertices"))
		remove_vertices = parse_value_string(vm["remove-vertices"].as<std::string>());

	// Various small flags

	if(vm.count("handle-creases"))
		subdivision_algorithm->set_crease_handling_flag();

	if(vm.count("geometric"))
		subdivision_algorithm->set_geometric_point_creation_flag();

	if(vm.count("preserve-boundaries"))
		subdivision_algorithm->set_boundary_preservation_flag();

	if(vm.count("statistics"))
		subdivision_algorithm->set_statistics_flag();

	// This only works for B-spline-based subdivision algorithms, hence the
	// dynamic_cast.
	if(vm.count("b-spline-weights"))
	{
		psalm::BsplineSubdivisionAlgorithm* b_spline_algorithm = dynamic_cast<psalm::BsplineSubdivisionAlgorithm*>(subdivision_algorithm);
		if(b_spline_algorithm)
			b_spline_algorithm->set_bspline_weights_usage();
	}

	// Read further command-line parameters; these are all supposed to be
	// input files. If the user already specified an output file, only one
	// input file will be accepted.

	std::vector<std::string> files;
	if(vm.count("input"))
	{
		files = vm["input"].as< std::vector<std::string> >();
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

		// It is possible that the user did not choose a subdivision
		// algorithm. psalm tries to work as a mesh converter in this
		// instance.
		if(subdivision_algorithm)
			subdivision_algorithm->apply_to(scene_mesh, steps);

		// Ditto for the fairing algorithm.
		if(fairing_algorithm)
			fairing_algorithm->apply_to(scene_mesh);

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

	delete(subdivision_algorithm);
	delete(fairing_algorithm);

	return(0);
}
