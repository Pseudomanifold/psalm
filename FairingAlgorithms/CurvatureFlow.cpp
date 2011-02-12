/*!
*	@file	CurvatureFlow.cpp
*	@brief	Implementation of a curvature flow algorithm
*/

#include "CurvatureFlow.h"

namespace psalm
{

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

	// We iterate over all faces and calculate the contributions of each
	// vertex of the face to the corresponding entry of the matrix

	for(size_t i = 0; i < input_mesh.num_faces(); i++)
	{
		face* f = input_mesh.get_face(i);
		for(size_t j = 0; j < f->num_vertices(); j++)
		{
			vertex* v = f->get_vertex(j);
			std::vector<vertex*> neighbours = v->get_neighbours();

			// FIXME: Used to update the correct matrix entries
			// below. This assumes that the IDs have been allocated
			// sequentially.
			size_t cur_id = v->get_id();

			// Find "opposing angles" for all neighbours; these are
			// the $\alpha_{ij}$ and $\beta_{ij}$ values used for
			// calculating the discrete curvature

			for(size_t k = 0; k < neighbours.size(); k++)
			{
				std::pair<double, double> angles = v->find_opposite_angles(neighbours[k]);
				if(angles.first >= 0.0 && angles.second >= 0.0)
				{
					// calculate contribution to matrix entries

					double contribution = 1.0/tan(angles.first) + 1.0/tan(angles.second);

					K(cur_id, cur_id)			+= contribution;
					K(cur_id, neighbours[k]->get_id())	-= contribution;
				}
			}
		}
	}

	// Scale the ith row of the matrix by the Voronoi area around the ith vertex

	for(size_t i = 0; i < input_mesh.num_vertices(); i++)
	{
		double area = input_mesh.get_vertex(i)->calc_voronoi_area();
		if(area < 2*std::numeric_limits<double>::epsilon())
		{
			// skip on error or upon encountering a Voronoi area
			// that is too small
			continue;
		}

		for(size_t j = 0; j < input_mesh.num_vertices(); j++)
		{
			if(K(i,j) != 0)
				K(i,j) *= area;
		}
	}

	return(K);
}

} // end of namespace "psalm"
