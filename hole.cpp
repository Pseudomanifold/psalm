/*!
*	@file	hole.cpp
*	@brief	Functions for representing a polygonal hole
*/

#include <limits>
#include <cmath>
#include "hole.h"

namespace psalm
{

/*!
*	Initializes a hole by creating objects of type vertex* from a vector of
*	vertex positions (represented as 3D points). Any old data will be
*	removed upon calling this function.
*
*	The hole is stored _without_ any connectivity information, so the mesh
*	only contains a list of vertices after calling this function.
*
*	@param vertices List of vertex positions
*/

void hole::initialize(const std::vector<v3ctor>& vertices)
{
	destroy();
	for(std::vector<v3ctor>::const_iterator vertex = vertices.begin(); vertex < vertices.end(); vertex++)
		add_vertex(*vertex);
}

/*!
*	Triangulates the hole using a minimum-weight triangulation algorithm.
*/

void hole::triangulate()
{
	size_t n = V.size();

	indices = new size_t*[n];		// store minimum indices (private member)
	double** weights = new double*[n];	// store weights of triangulation (only required locally)

	for(size_t i = 0; i < n; i++)
	{
		indices[i] = new size_t[n];
		weights[i] = new double[n];

		// Initialize weights array correctly
		if(i < n-1)
			weights[i][i+1] = 0;
		if(i < n-2)
			weights[i][i+2] = objective_function(V[i], V[i+1], V[i+2]);
	}

	size_t j = 2;
	while(j++ < n-1)	// this is correct -- the loop is supposed to start
				// with j == 3
	{
		for(size_t i = 0; i < n-j; i++)
		{
			size_t k = i+j;

			// Find minimum
			double min_weight = std::numeric_limits<double>::max();
			size_t min_index = std::numeric_limits<size_t>::max();
			for(size_t m = i+1; m < k; m++)
			{
				double res = weights[i][m]+weights[m][k]+objective_function(V[i], V[m], V[k]);
				if(res < min_weight)
				{
					min_weight = res;
					min_index = m;
				}
			}

			weights[i][k] = min_weight;
			indices[i][k] = min_index;
		}
	}

	// Now weights[0][n-1] contains the weight of the minimal
	// triangulation. Construct triangulation using the stored indices.
	construct_triangulation(0, n-1);

	for(size_t i = 0; i < n; i++)
	{
		delete[] weights[i];
		delete[] indices[i];
	}

	delete[] weights;
	delete[] indices;
}

/*!
*	Using the array of indices that achieve the minimum of the
*	minimum-weight triangulation, this function constructs the actual
*	triangulation of the mesh.
*
*	This function calls itself recursively.
*
*	@param i Smallest index for constructing a triangle
*	@param k Largest index for constructing a triangle
*/

void hole::construct_triangulation(size_t i, size_t k)
{
	// abort
	if(i+2 == k)
		add_face(V[i], V[i+1], V[k]);

	// use minimum index to branch off
	else
	{
		size_t j = indices[i][k];
		if(j != i+1)
			construct_triangulation(i, j);

		add_face(V[i], V[j], V[k]);

		if(j != k-1)
			construct_triangulation(j, k);

	}
}

/*!
*	Objective function for the minimum-weight triangulation. The area of
*	the triangle is calculated, while "skinny" triangles are penalized.
*
*	@param v1	1st vertex of triangle
*	@param v2	2nd vertex of triangle
*	@param v3	3rd vertex of triangle
*
*	@returns	Area of triangle[v1, v2, v3] with penalization of
*			"skinny" triangles.
*/

double hole::objective_function(const vertex* v1, const vertex* v2, const vertex* v3) const
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
