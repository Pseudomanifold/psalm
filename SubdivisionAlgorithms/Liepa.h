/*!
*	@file	Liepa.h
*	@brief	Class describing Liepa's subdivision scheme
*/

#ifndef __LIEPA_H__
#define __LIEPA_H__

#include "SubdivisionAlgorithm.h"

namespace psalm
{

/*!
*	@class Liepa
*	@brief Liepa subdivision algorithm
*/

class Liepa : public SubdivisionAlgorithm
{
	public:
		bool apply_to(mesh& input_mesh);
};

} // end of namespace "psalm"

#endif
