/*!
	@file	MinimumWeightTriangulation.h
	@brief	Class describing a generical minimum weight triangulation algorithm
*/

#ifndef __MINIMUM_WEIGHT_TRIANGULATION_H__
#define __MINIMUM_WEIGHT_TRIANGULATION_H__

#include <cmath>

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

#include "TriangulationAlgorithm.h"

namespace psalm
{

/*!
*	Typedef for the standard tuple used below.
*/

typedef boost::tuple<double, double> ktuple;

/*!
*	@class MinimumWeightTriangulation
*	@brief Generical minimum weight triangulation class
*/

class MinimumWeightTriangulation : public TriangulationAlgorithm
{
	public:
		MinimumWeightTriangulation();

		bool apply_to(mesh& input_mesh);

	protected:
		size_t** indices;	///< Stores the index that achieves the
					///< minimum for the minimum-weight
					///< triangulation.

		bool construct_triangulation(mesh& input_mesh, size_t i, size_t k);

		ktuple (*objective_function)(const vertex* v1, const vertex* v2, const vertex* v3);

		static ktuple minimum_area(const vertex* v1, const vertex* v2, const vertex* v3);
		static ktuple minimum_area_and_angle(const vertex* v1, const vertex* v2, const vertex* v3);
		static ktuple minimum_area_and_normal_angle(const vertex* v1, const vertex* v2, const vertex* v3);
};

/*!
*	Objective function for the minimum-weight triangulation. Calculates the
*	area of the triangle.
*
*	@param v1	1st vertex of triangle
*	@param v2	2nd vertex of triangle
*	@param v3	3rd vertex of triangle
*
*	@returns	Area of triangle[v1, v2, v3]. Second component of the
*			tuple is set to 0.0.
*/

inline ktuple MinimumWeightTriangulation::minimum_area(const vertex* v1, const vertex* v2, const vertex* v3)
{
	if(v1 == NULL || v2 == NULL || v3 == NULL)
		return(boost::tuple<double,double>(std::numeric_limits<double>::max(), 0.0));	// ensure that invalid data does not
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

	return(boost::tuple<double, double>(area, 0.0));
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
*			for skinny triangles. Second component of the tuple is
*			set to 0.0.
*/

inline ktuple MinimumWeightTriangulation::minimum_area_and_angle(const vertex* v1, const vertex* v2, const vertex* v3)
{
	if(v1 == NULL || v2 == NULL || v3 == NULL)
		return(boost::tuple<double,double>(std::numeric_limits<double>::max(), 0.0));	// ensure that invalid data does not
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

	return(boost::tuple<double, double>(area, 0.0));
}

/*!
*	Objective function for the minimum-weight triangulation. Calculates the
*	area of the triangle and the maximum angle between the normal of the
*	triangle and the normals of its vertices.
*
*	@param v1	1st vertex of triangle
*	@param v2	2nd vertex of triangle
*	@param v3	3rd vertex of triangle
*
*	@returns	Area of triangle[v1, v2, v3]. Second component of the
*			tuple is the maximum angle between the triangle normal
*			and the normals of its vertices.
*/

inline ktuple MinimumWeightTriangulation::minimum_area_and_normal_angle(const vertex* v1, const vertex* v2, const vertex* v3)
{
	if(v1 == NULL || v2 == NULL || v3 == NULL)
		return(boost::tuple<double,double>(	std::numeric_limits<double>::max(),
							std::numeric_limits<double>::max()));	// ensure that invalid data does not
												// appear in the weighted triangulation
	// Calculate normal of current triangle and angles with the normals of
	// the vertices of the triangle

	const v3ctor& A = v1->get_position();
	const v3ctor& B = v2->get_position();
	const v3ctor& C = v3->get_position();

	v3ctor normal = ((B-A)|(C-A)).normalize();

	// Use _maximum_ angle between normal of the triangle and the normal of
	// the triangle
	double angle;
	angle = std::max(acos(normal*v1->get_normal().normalize()), acos(normal*v2->get_normal().normalize()));
	angle = std::max(acos(normal*v3->get_normal().normalize()), angle);

	double area	= minimum_area(v1, v2, v3).get<0>();

	return(boost::tuple<double, double>(angle, area));
}

} // end of namespace "psalm"

#endif
