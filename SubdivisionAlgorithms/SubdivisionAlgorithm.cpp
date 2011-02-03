/*!
*	@file	SubdivisionAlgorithm.cpp
*	@brief	Functions for general subdivision algorithm class
*/

#include <iostream>
#include <iomanip>

#include "SubdivisionAlgorithm.h"

namespace psalm
{

/*!
*	Constructor for setting some default values
*/

SubdivisionAlgorithm::SubdivisionAlgorithm()
{
	handle_creases		= false;
	preserve_boundaries	= false;
	print_statistics	= false;
}

/*!
*	Empty destructor
*/

SubdivisionAlgorithm::~SubdivisionAlgorithm()
{
}

/*!
*	Sets or unsets flag signalling geometric point creation. The effect of
*	this flag depends on the subdivision algorithm. Some algorithms are
*	implemented without distinguishing between geometric and parametric
*	point creation. In these cases, for example, the flag will not have any
*	effect.
*
*	@param value New value for flag
*/

void SubdivisionAlgorithm::set_geometric_point_creation_flag(bool value)
{
	use_geometric_point_creation = value;
}

/*!
*	@returns Current value of flag signalling geometric point creation.
*/

bool SubdivisionAlgorithm::get_geometric_point_creation_flag()
{
	return(use_geometric_point_creation);
}

/*!
*	Sets or unsets flag signalling the preservation of boundaries. The
*	effects of this flag depend on the subdivision algorithm.
*
*	@param value New value for the flag
*/

void SubdivisionAlgorithm::set_boundary_preservation_flag(bool value)
{
	preserve_boundaries = value;
}

/*!
*	@returns Current value of flag signalling the preservation of
*	boundaries.
*/

bool SubdivisionAlgorithm::get_boundary_preservation_flag()
{
	return(preserve_boundaries);
}

/*!
*	Sets current value of flag signalling crease handling. The effect of
*	this flag depends on the subdivision algorithm.
*
*	@param value New value for flag
*/

void SubdivisionAlgorithm::set_crease_handling_flag(bool value)
{
	handle_creases = value;
}

/*!
*	@returns Current value of flag signalling crease handling.
*/

bool SubdivisionAlgorithm::get_crease_handling_flag()
{
	return(handle_creases);
}

/*!
*	Sets flag signalling that statistics should be written to STDOUT.
*
*	@param value New value for flag
*/

void SubdivisionAlgorithm::set_statistics_flag(bool value)
{
	print_statistics = value;
}

/*!
*	@returns Current value of flag signalling that statistics should be
*	written to STDOUT.
*/

bool SubdivisionAlgorithm::get_statistics_flag()
{
	return(print_statistics);
}

/*!
*	Generic function for applying a subdivision algorithm a number of times
*	to a certain mesh. The function is simply a wrapper for the virtual
*	SubdivisionAlgorithm::apply_to() function, which is called #steps
*	times. Statistics are generated if the flag for printing statistics is
*	set.
*
*	@param	input_mesh	Mesh on which the algorithm is applied
*	@param	steps		Number of steps
*
*	@return	true on success, else false
*/

bool SubdivisionAlgorithm::apply_to(mesh& input_mesh, size_t steps)
{
	size_t num_vertices	= input_mesh.num_vertices();
	size_t num_edges	= input_mesh.num_edges();
	size_t num_faces	= input_mesh.num_faces();

	bool res = true;

	clock_t start	= clock();
	size_t width	= static_cast<unsigned int>(log10(steps))*2;
	for(size_t i = 0; i < steps; i++)
	{
		if(print_statistics)
			std::cerr << "[" << std::setw(width) << i << "]\n";

		res = (res && this->apply_to(input_mesh));

		if(print_statistics)
			std::cerr << "\n";
	}
	clock_t end = clock();

	if(print_statistics)
	{
		std::cerr	<< std::setfill('-') << std::setw(78) << "\n"
				<< "PSALM STATISTICS\n"
				<< std::setfill('-') << std::setw(80) << "\n\n\n"
				<< "BEFORE:\n"
				<< std::setfill(' ')
				<< std::left
				<< std::setw(30) << "\tNumber of vertices: " << num_vertices << "\n"
				<< std::setw(30) << "\tNumber of edges: " << num_edges << "\n"
				<< std::setw(30) << "\tNumber of faces: " << num_faces << "\n\n\n"
				<< "AFTER:\n"
				<< std::setw(30) << "\tNumber of vertices: "	<< input_mesh.num_vertices()	<< "\n"
				<< std::setw(30) << "\tNumber of edges: "	<< input_mesh.num_edges()	<< "\n"
				<< std::setw(30) << "\tNumber of faces: "	<< input_mesh.num_faces()	<< "\n\n\n"
				<< "TOTAL CPU TIME: "
				<< (static_cast<double>(end-start)/CLOCKS_PER_SEC)
				<< "s\n\n";
	}

	return(res);
}

} // end of namespace "psalm"
