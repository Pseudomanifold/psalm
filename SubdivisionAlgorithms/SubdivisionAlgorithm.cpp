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
*	Prints a progress bar to STDOUT.
*
*	@param	op	Operation the progress bar shall show; will be expanded by ": "
*	@param	cur_pos	Current position of progress bar
*	@param	max_pos	Maximum position of progress bar
*/

void SubdivisionAlgorithm::print_progress(std::string message, size_t cur_pos, size_t max_pos)
{
	size_t percentage = (cur_pos*100)/max_pos;
	static size_t last;

	if(percentage - last < 5 && cur_pos != max_pos)
		return;

	std::cerr	<< "\r" << std::left << std::setw(50) << message << ": "
			<< "[";


	std::cerr	<< std::setw(10)
			<< std::string( percentage/10, '#')
			<< "]"
			<< " "
			<< std::setw(3) << percentage << "%" << std::right;

	if(cur_pos == max_pos)
		std::cerr << std::endl;

	last = percentage;
}

} // end of namespace "psalm"
