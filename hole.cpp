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
*	@param vertices List of vertex positions and vertex IDs
*/

void hole::initialize(const std::vector< std::pair<v3ctor, size_t> >& indexed_vertices)
{
	destroy();
	size_t max_id = 0;
	for(std::vector< std::pair<v3ctor, size_t> >::const_iterator indexed_vertex = indexed_vertices.begin(); indexed_vertex < indexed_vertices.end(); indexed_vertex++)
	{
		add_vertex(indexed_vertex->first, indexed_vertex->second);
		if(indexed_vertex->second > max_id)
			max_id = indexed_vertex->second;
	}

	// The IDs of new vertices must be larger than the IDs of their
	// predecessors. Otherwise, ID clashes will occur. The id_offset is
	// used for every mesh::add_vertex() operation.
	id_offset = max_id;
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

	/*
		Mark _all_ vertices as boundary vertices. Upon subdivision, the
		boundary vertices will not be changed and we need to identify
		them when storing the subdivided mesh.
	*/

	for(std::vector<vertex*>::iterator v_it = V.begin(); v_it < V.end(); v_it++)
		(*v_it)->set_on_boundary();
}

/*!
*	Saves the current hole in a raw format, i.e. all coordinate and
*	connectivity information is stored in arrays. The caller of this method
*	is notified about the number of new vertices and new faces created by
*	this method. New vertices are recognized by checking their boundary
*	flags. All boundary vertices are old vertices and will _not_ be
*	reported by this function.
*
*	This function has been implemented in class hole because we may assume
*	that the triangulated hole consists of triangles only.
*
*	@param num_new_vertices	Number of new (i.e. non-boundary) vertices
*	@param new_coordinates	Array of new coordinates
*	@param num_faces	Number of faces in the mesh
*	@param vertex_IDs	Face connectivity information -- a negative ID
*				signifies an old vertex. This has to be taken
*				into account by the caller.
*/

void hole::save_raw_data(int* num_new_vertices, double** new_coordinates, int* num_faces, long** vertex_IDs)
{
	size_t num_boundary_vertices = 0;		// count boundary vertices to obtain correct IDs
	std::vector<const vertex*> new_vertices;	// stores new vertices

	// Count new vertices and store them

	for(std::vector<vertex*>::const_iterator v_it = V.begin(); v_it < V.end(); v_it++)
	{
		vertex* v = *v_it;

		// Boundary vertex, hence an _old_ vertex, i.e. one that is
		// already known by the caller
		if(v->is_on_boundary())
			num_boundary_vertices++;

		// New vertex
		else
			new_vertices.push_back(v);
	}

	*new_coordinates = new double[3*new_vertices.size()];
	for(size_t position = 0; position < 3*new_vertices.size(); position++)
	{
		const v3ctor& v = new_vertices[position]->get_position();

		*new_coordinates[position*3]	= v[0];
		*new_coordinates[position*3+1]	= v[1];
		*new_coordinates[position*3+2]	= v[2];
	}

	// Allocate storage for new faces and store them -- the IDs of their
	// vertices need to be changed

	*num_faces = F.size();
	*vertex_IDs = new long[3*F.size()];
	for(size_t face_index = 0; face_index < F.size(); face_index++)
	{
		face* f = F[face_index];

		// Hole is assumed to consist of triangular faces only
		if(f->num_vertices() != 3)
			throw(std::runtime_error("hole::save_raw_data(): Unable to handle non-triangular faces"));

		for(size_t i = 0; i < 3; i++)
		{
			vertex* v = f->get_vertex(i);

			// Store negative IDs for old vertices
			if(v->is_on_boundary())
				*vertex_IDs[3*face_index+i] = static_cast<long>(-1*v->get_id());
			else
			{
				// Store zero-indexed IDs for new vertices. For
				// this purpose, the offset needs to be
				// subtracted. This yields vertex IDs in the
				// range of [num_boundary, ...]. Hence, the
				// range needs to be adjusted by subtracting
				// num_boundary_vertices

				*vertex_IDs[3*face_index+i] = static_cast<long>(v->get_id() - id_offset - num_boundary_vertices);
			}
		}
	}
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
