/*!
*	@file	PlanarSegmentation.h
*	@brief	Class describing a planar segmentation algorithm
*/

#ifndef __PLANAR_SEGMENTATION_H__
#define __PLANAR_SEGMENTATION_H__

#include "SegmentationAlgorithm.h"

namespace psalm
{

/*!
*	@class	PlanarSegmentation
*	@brief	Describes algorithm for performing a planar segmentation of an
*		input mesh
*/

class PlanarSegmentation : public SegmentationAlgorithm
{
	public:
		mesh apply_to(mesh& input_mesh);
};

} // end of namespace "psalm"

#endif
