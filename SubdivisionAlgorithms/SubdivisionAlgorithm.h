/*!
*	@file	SubdivisionAlgorithm.h
*	@brief	Contains class describing general subdivision algorithms
*/

#ifndef __SUBDIVISION_ALGORITHM_H__
#define __SUBDIVISION_ALGORITHM_H__

#include <iomanip>
#include <string>
#include <cmath>

#include "mesh.h"

namespace psalm
{

/*!
*       @class SubdivisionAlgorithm
*       @brief Abstract base class for subdivision algorithms
*/

class SubdivisionAlgorithm
{
        public:
                SubdivisionAlgorithm();
                virtual ~SubdivisionAlgorithm();

		bool apply_to(mesh& M, size_t steps);
		virtual bool apply_to(mesh& M) = 0;

		enum weights
		{
			catmull_clark,
			doo_sabin,
			degenerate
		};

		virtual bool set_weights(weights new_weights) = 0;

		// Here be flags...

		void set_crease_handling_flag(bool value = true);
		bool get_crease_handling_flag();

		void set_boundary_preservation_flag(bool value = true);
		bool get_boundary_preservation_flag();

		void set_geometric_point_creation_flag(bool value = true);
		bool get_geometric_point_creation_flag();

		void set_statistics_flag(bool value = true);
		bool get_statistics_flag();

	protected:
		void print_progress(std::string op, size_t cur_pos, size_t max_pos);

		bool preserve_boundaries;	///< Flag signalling that boundaries of open meshes need to be preserved
		bool handle_creases;		///< Flag signalling that creases should be handled instead of ignored
		bool print_statistics;		///< Flag signalling that the algorithm should write its progress to STDERR

		/*!
			Flag signalling that new face vertices are supposed to
			be created geometrically instead of using parametrical
			(i.e. weight-based) methods.
		*/

		bool use_geometric_point_creation;
};

/*!
*	Prints a progress bar to STDOUT.
*
*	@param	op	Operation the progress bar shall show; will be expanded by ": "
*	@param	cur_pos	Current position of progress bar
*	@param	max_pos	Maximum position of progress bar
*/

inline void SubdivisionAlgorithm::print_progress(std::string message, size_t cur_pos, size_t max_pos)
{
	if(!print_statistics)
		return;

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

#endif
