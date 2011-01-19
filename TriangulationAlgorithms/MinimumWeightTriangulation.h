/*!
	@file	MinimumWeightTriangulation.h
	@brief	Class describing a generical minimum weight triangulation algorithm
*/

#ifndef __MINIMUM_WEIGHT_TRIANGULATION_H__
#define __MINIMUM_WEIGHT_TRIANGULATION_H__

#include "TriangulationAlgorithm.h"

namespace psalm
{

/*!
*	@class MinimumWeightTriangulation
*	@brief Generical minimum weight triangulation class
*/

class MinimumWeightTriangulation : public TriangulationAlgorithm
{
	public:
		bool apply_to(mesh& input_mesh);

	protected:
		size_t** indices;	///< Stores the index that achieves the
					///< minimum for the minimum-weight
					///< triangulation.

		bool construct_triangulation(mesh& input_mesh, size_t i, size_t k);

		double (*objective_function)(const vertex* v1, const vertex* v2, const vertex* v3);

		static double minimum_area(const vertex* v1, const vertex* v2, const vertex* v3);
		static double minimum_area_and_angle(const vertex* v1, const vertex* v2, const vertex* v3);
};

} // end of namespace "psalm"

#endif
