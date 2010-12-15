/*!
*	@file	SubdivisionAlgorithm.h
*	@brief	Contains class describing general subdivision algorithms
*/

#ifndef __SUBDIVISION_ALGORITHM_H__
#define __SUBDIVISION_ALGORITHM_H__

#include <string>

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

		virtual bool apply_to(mesh& M) = 0;

	protected:
		void print_progress(std::string op, size_t cur_pos, size_t max_pos);

		bool preserve_boundaries;	// TODO: Set in constructor; allow getters and setters
		bool handle_creases;		// TODO: ditto
};

} // end of namespace "psalm"

#endif
