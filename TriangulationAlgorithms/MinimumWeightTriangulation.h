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

/*!
*	Objective function for the minimum-weight triangulation. Calculates the
*	area of the triangle.
*
*	@param v1	1st vertex of triangle
*	@param v2	2nd vertex of triangle
*	@param v3	3rd vertex of triangle
*
*	@returns	Area of triangle[v1, v2, v3]
*/

inline double MinimumWeightTriangulation::minimum_area(const vertex* v1, const vertex* v2, const vertex* v3)
{
	if(v1 == NULL || v2 == NULL || v3 == NULL)
		return(std::numeric_limits<double>::max());	// ensure that invalid data does not
								// appear in the weighted triangulation

	const v3ctor& A = v1->get_position();
	const v3ctor& B = v2->get_position();
	const v3ctor& C = v3->get_position();

	v3ctor BA = B-A;
	v3ctor CA = C-A;

	// This formula is wrong by design. Multiplication with 0.5 is _not_
	// necessary because the objective function is minimized and we are not
	// interested in the _absolute_ values, anyway.
	double area	= (BA|CA).length();

	/*

	FIXME

	// angles of the triangle
	double alpha	= (BA).normalize()*(CA).normalize();
	double beta	= (A-B).normalize()*(C-B).normalize();
	double gamma	= (B-C).normalize()*(A-C).normalize();

	double penalty = std::max(alpha, std::max(beta, gamma))/cos(M_PI/3.0);
	return(area*penalty);

	*/

	return(area);
}

/*!
*	Objective function for the minimum-weight triangulation. Calculates the
*	area of the triangle and tries to penalize "skinny triangles" by
*	observing the interior angles.
*
*	@param v1	1st vertex of triangle
*	@param v2	2nd vertex of triangle
*	@param v3	3rd vertex of triangle
*
*	@returns	Area of triangle[v1, v2, v3] weighted with a penalty
*			for skinny triangles.
*/

inline double MinimumWeightTriangulation::minimum_area_and_angle(const vertex* v1, const vertex* v2, const vertex* v3)
{
	if(v1 == NULL || v2 == NULL || v3 == NULL)
		return(std::numeric_limits<double>::max());	// ensure that invalid data does not
								// appear in the weighted triangulation

	const v3ctor& A = v1->get_position();
	const v3ctor& B = v2->get_position();
	const v3ctor& C = v3->get_position();

	v3ctor BA = B-A;
	v3ctor CA = C-A;

	// This formula is wrong by design. Multiplication with 0.5 is _not_
	// necessary because the objective function is minimized and we are not
	// interested in the _absolute_ values, anyway.
	double area	= (BA|CA).length();

	/*

	FIXME

	// angles of the triangle
	double alpha	= (BA).normalize()*(CA).normalize();
	double beta	= (A-B).normalize()*(C-B).normalize();
	double gamma	= (B-C).normalize()*(A-C).normalize();

	double penalty = std::max(alpha, std::max(beta, gamma))/cos(M_PI/3.0);
	return(area*penalty);

	*/

	return(area);
}
} // end of namespace "psalm"

#endif
