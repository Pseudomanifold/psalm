/*!
*	@file	FairingAlgorithm.h
*	@brief	Generic (abstract) class for fairing algorithms
*/

#ifndef __FAIRING_ALGORITHM_H__
#define __FAIRING_ALGORITHM_H__

#include "mesh.h"

namespace psalm
{

/*!
*	@class FairingAlgorithm
*	@brief Generic fairing algorithm class (containing utility functions)
*/

class FairingAlgorithm
{
	public:
		FairingAlgorithm();
                virtual ~FairingAlgorithm();

		virtual bool apply_to(mesh& input_mesh) = 0;
};

} // end of namespace "psalm"

#endif
