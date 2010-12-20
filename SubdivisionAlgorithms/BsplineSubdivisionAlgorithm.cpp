#include "BsplineSubdivisionAlgorithm.h"

namespace psalm
{

/*!
*	Sets default values for B-spline-based algorithms.
*/

BsplineSubdivisionAlgorithm::BsplineSubdivisionAlgorithm()
{
	use_bspline_weights		= false;
	use_geometric_point_creation	= false;
}

/*!
*	Sets or unsets flag signalling the usage of B-spline weights.
*
*	@param usage New value for flag
*/

void BsplineSubdivisionAlgorithm::set_bspline_weights_usage(bool usage)
{
	use_bspline_weights = usage;
}

/*!
*	@returns Current value of flag signalling the usage of B-spline weights.
*/

bool BsplineSubdivisionAlgorithm::get_bspline_weights_usage()
{
	return(use_bspline_weights);
}

/*!
*	Sets or unsets flag signalling geometric point creation.
*
*	@param status New value for flag
*/

void BsplineSubdivisionAlgorithm::set_geometric_point_creation_flag(bool status)
{
	use_geometric_point_creation = status;
}

/*!
*	@returns Current value of flag signalling geometric point creation.
*/

bool BsplineSubdivisionAlgorithm::get_geometric_point_creation_flag()
{
	return(use_geometric_point_creation);
}

} // end of namespace "psalm"
