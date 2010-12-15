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
*	Empty constructor
*/

SubdivisionAlgorithm::SubdivisionAlgorithm()
{
}

/*!
*	Empty destructor
*/

SubdivisionAlgorithm::~SubdivisionAlgorithm()
{
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
