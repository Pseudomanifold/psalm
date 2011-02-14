/*!
*	@file	CurvatureFlow.cpp
*	@brief	Implementation of a curvature flow algorithm
*/

#include "CurvatureFlow.h"

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include <boost/numeric/ublas/io.hpp>

namespace psalm
{

/*!
*	Empty constructor
*/

CurvatureFlow::CurvatureFlow()
{
}

/*!
*	Applies the curvature flow algorithm to the vertices of a given mesh.
*	The size of timesteps needs to be set before. The input mesh is
*	irreversibly changed by this operation.
*
*	@param	input_mesh Mesh on which the algorithm works.
*	@return	false if an error occurred, else true
*/

bool CurvatureFlow::apply_to(mesh& input_mesh)
{
	using namespace boost::numeric::ublas;

	size_t n = input_mesh.num_vertices();
	if(n == 0)
		return(true); // silently ignore empty meshes

	// Stores x,y,z components of the vertices in the mesh
	vector<double> X(n);
	vector<double> Y(n);
	vector<double> Z(n);

	// Fill vector with position data
	for(size_t i = 0; i < n; i++)
	{
		const v3ctor& pos = input_mesh.get_vertex(i)->get_position();

		X[i] = pos[0];
		Y[i] = pos[1];
		Z[i] = pos[2];
	}

	size_t step = 0;
	while(step++ < 20)
	{
		// Prepare for "solving" the linear system (for now, this is something
		// akin to the explicit Euler method)

		mapped_matrix<double> M(n, n);	// transformed matrix for the solving
						// process, i.e. id - dt*K, where K is
						// the matrix of the curvature operator

		// FIXME: Should be user-configurable
		double dt = 0.05;

		M = identity_matrix<double>(n, n) - dt*calc_curvature_operator(input_mesh);

		// Solve x,y,z components independently. This is _hellishly_ slow, but
		// sufficient for small meshes.

		X = prod(M, X);
		Y = prod(M, Y);
		Z = prod(M, Z);
	}

	for(size_t i = 0; i < n; i++)
		input_mesh.get_vertex(i)->set_position(X[i], Y[i], Z[i]);

	return(true);
}

/*!
*	Given an input mesh, calculates the curvature operator matrix for this
*	mesh. The matrix will be a _sparse_ matrix, hence the need for handling
*	it via boost.
*
*	@param	input_mesh Mesh to be processed
*	@return	Sparse matrix describing the curvature operator
*/

boost::numeric::ublas::mapped_matrix<double> CurvatureFlow::calc_curvature_operator(mesh& input_mesh)
{
	using namespace boost::numeric::ublas;
	mapped_matrix<double> K(input_mesh.num_vertices(), input_mesh.num_vertices()); // K as in "Kurvature"...

	// We iterate over all vertices and calculate the contributions of each
	// vertex to the corresponding entry of the matrix

	for(size_t i = 0; i < input_mesh.num_vertices(); i++)
	{
		vertex* v = input_mesh.get_vertex(i);
		std::vector<const vertex*> neighbours = v->get_neighbours();

		// FIXME: Used to update the correct matrix entries
		// below. This assumes that the IDs have been allocated
		// sequentially.
		size_t cur_id = v->get_id();

		// Find "opposing angles" for all neighbours; these are
		// the $\alpha_{ij}$ and $\beta_{ij}$ values used for
		// calculating the discrete curvature

		for(size_t j = 0; j < neighbours.size(); j++)
		{
			std::pair<double, double> angles = v->find_opposite_angles(neighbours[j]);
			if(angles.first >= 0.0 && angles.second >= 0.0)
			{
				// calculate contribution to matrix entries

				double contribution = 1.0/tan(angles.first) + 1.0/tan(angles.second);

				K(cur_id, cur_id)			+= contribution;
				K(cur_id, neighbours[j]->get_id())	-= contribution;
			}
		}
	}

	// Scale the ith row of the matrix by the Voronoi area around the ith
	// vertex; this works because the _first_ iterator of all matrix types
	// is dense, whereas the second iterator is sparse in this case

	size_t i = 0;
	for(mapped_matrix<double>::iterator1 it1 = K.begin1(); it1 != K.end1(); it1++)
	{
		double area = input_mesh.get_vertex(i)->calc_voronoi_area();
		if(area < 2*std::numeric_limits<double>::epsilon())
		{
			// skip on error or upon encountering a Voronoi area
			// that is too small
			i++;
			continue;
		}

		for(mapped_matrix<double>::iterator2 it2 = it1.begin(); it2 != it1.end(); it2++)
			(*it2) /= 4.0*area;

		i++;
	}

	return(K);
}

} // end of namespace "psalm"
