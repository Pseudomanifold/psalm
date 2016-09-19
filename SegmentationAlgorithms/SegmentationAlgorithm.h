/*!
*	@file	SegmentationAlgorithm.h
*	@brief	Generic (abstract) class for segmentation algorithms
*/

#ifndef __SEGMENTATION_ALGORITHM_H__
#define __SEGMENTATION_ALGORITHM_H__

#include "mesh.h"

namespace psalm
{

/*!
*	@class SegmentationAlgorithm
*	@brief Generic segmentation algorithm class
*/

class SegmentationAlgorithm
{
	public:
		SegmentationAlgorithm();

		virtual mesh apply_to(mesh& input_mesh) = 0;
};

} // end of namespace "psalm"

#endif
