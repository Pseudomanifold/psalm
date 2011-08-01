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
	
	private:
		void label_planar_vertices(mesh& input_mesh);
		void label_nonplanar_faces(mesh& input_mesh);
		void label_regions(mesh& input_mesh);

		std::vector<vertex*>	planar_vertices;
		std::vector<vertex*>	nonplanar_vertices;
		std::vector<face*>	nonplanar_faces;
};

} // end of namespace "psalm"

#endif
