/*!
*	@file	TriangulationAlgorithm.h
*	@brief	Contains class describing general triangulation algorithms
*/

#ifndef __TRIANGULATION_ALGORITHM_H__
#define __TRIANGULATION_ALGORITHM_H__

#include "mesh.h"

namespace psalm
{

/*!
*       @class TriangulationAlgorithm
*       @brief Abstract base class for triangulation algorithms
*/

class TriangulationAlgorithm
{
        public:
                TriangulationAlgorithm();
                virtual ~TriangulationAlgorithm();

		virtual bool apply_to(mesh& M) = 0;
};

} // end of namespace "psalm"

#endif
