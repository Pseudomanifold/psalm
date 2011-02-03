/*!
*	@file	BsplineSubdivisionAlgorithm.h
*	@brief	Contains class describing B-spline-based subdivision algorithms
*/

#ifndef __BSPLINE_SUBDIVISION_ALGORITHM_H__
#define __BSPLINE_SUBDIVISION_ALGORITHM_H__

#include "SubdivisionAlgorithm.h"

namespace psalm
{

/*!
*	@class	BsplineSubdivisionAlgorithm
*	@brief	Abstract base class for B-spline-based subdivision algorithms
*/

class BsplineSubdivisionAlgorithm : public SubdivisionAlgorithm
{
	public:
		BsplineSubdivisionAlgorithm();

		void set_bspline_weights_usage(bool usage = true);
		bool get_bspline_weights_usage();

	protected:

		/*!
			Flag signalling that the default B-spline weights of an
			algorithm shall be used even if user-specified weights
			would supersede them.
		*/

		bool use_bspline_weights;
};

} // end of namespace "psalm"

#endif
