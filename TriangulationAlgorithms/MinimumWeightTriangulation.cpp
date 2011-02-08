/*!
*	@file	MinimumWeightTriangulation.cpp
*	@brief	Functions for minimum weight triangulation
*/

#include "MinimumWeightTriangulation.h"

namespace psalm
{

/*!
*	Sets default attributes for the minimum-weight triangulation algorithm:
*	The objective function is set to be a minimum-area function.
*/

MinimumWeightTriangulation::MinimumWeightTriangulation()
{
	objective_function = minimum_area;
}

/*!
*	Applies triangulation algorithm to the input mesh. The function
*	requires the input mesh to consist solely of unconnected points. If
*	this is not the case, the function will abort.
*
*	@param input_mesh Mesh that will be triangulated.
*
*	@return true if the mesh could be triangulated, else false. Errors may
*	occur when the topology of the mesh is already defined.
*/

bool MinimumWeightTriangulation::apply_to(mesh& input_mesh)
{
	if(	input_mesh.num_faces() > 0 ||
		input_mesh.num_edges() > 0)
		return(false);

	size_t n = input_mesh.num_vertices();
	if(n < 3)
		return(false);

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
			weights[i][i+2] = objective_function(	input_mesh.get_vertex(i),
								input_mesh.get_vertex(i+1),
								input_mesh.get_vertex(i+2));
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
				double res =	weights[i][m]+
						weights[m][k]+objective_function(	input_mesh.get_vertex(i),
											input_mesh.get_vertex(m),
											input_mesh.get_vertex(k));

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
	bool result = construct_triangulation(input_mesh, 0, n-1);

	for(size_t i = 0; i < n; i++)
	{
		delete[] weights[i];
		delete[] indices[i];
	}

	delete[] weights;
	delete[] indices;

	/*
		Mark _all_ vertices as boundary vertices. Upon subdivision, the
		boundary vertices will not be changed and we need to identify
		them when storing the subdivided mesh.
	*/

	for(size_t i = 0; i < input_mesh.num_vertices(); i++)
		input_mesh.get_vertex(i)->set_on_boundary();

	return(result);
}

/*!
*	Using the array of indices that achieve the minimum of the
*	minimum-weight triangulation, this function constructs the actual
*	triangulation of the mesh.
*
*	This function calls itself recursively.
*
*	@param input_mesh Mesh for which the triangles are constructed
*	@param i Smallest index for constructing a triangle
*	@param k Largest index for constructing a triangle
*
*	@return true if the triangulation could be constructed, else false.
*/

bool MinimumWeightTriangulation::construct_triangulation(mesh& input_mesh, size_t i, size_t k)
{
	// abort
	if(i+2 == k)
		input_mesh.add_face(	input_mesh.get_vertex(i),	// TODO: Check errors for mesh::add_face()
					input_mesh.get_vertex(i+1),
					input_mesh.get_vertex(k));

	// use minimum index to branch off
	else
	{
		size_t j = indices[i][k];
		if(j != i+1)
		{
			if(!construct_triangulation(input_mesh, i, j))
				return(false);
		}

		input_mesh.add_face(	input_mesh.get_vertex(i),	// TODO: Check errors for mesh::add_face()
					input_mesh.get_vertex(j),
					input_mesh.get_vertex(k));

		if(j != k-1)
		{
			if(!construct_triangulation(input_mesh, j, k))
				return(false);
		}
	}

	return(true);
}

} // end of namespace "psalm"
