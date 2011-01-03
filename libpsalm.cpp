/*!
*	@file	libpsalm.cpp
*	@brief	Exports a function for filling holes in meshes. Used by GigaMesh.
*	@author	Bastian Rieck <bastian.rieck@iwr.uni-heidelberg.de>
*/

#include <vector>

#include "libpsalm.h"
#include "hole.h"

/*!
*	Given a polygonal line described as a list of vertices, this function
*	triangulates the hole and subdivides it. Afterwards, the new data is
*	stored in some arrays given as parameters of the caller.
*
*	@param num_vertices	Number of vertices in polygonal line
*	@param vertex_IDs	List of vertex IDs, used to identify them
*	@param coordinates	Array of vertex coordinates (size:
*				3*num_vertices)
*
*	@param num_new_vertices	Number of new vertices created by the algorithm
*	@param new_coordinates	Array of vertex coordinates created by the
*				algorithm (size: 3*num_new_vertices)
*	@param num_new_faces	Number of new faces created by the algorithm
*	@param new_vertex_IDs	List of vertex IDs for the new faces (size:
*				3*num_new_faces). Negative IDs signify that the
*				vertex is _not_ new.
*
*	The nomenclature here might be confusing: new_vertex_IDs contains 3
*	vertex IDs that describe a new face created by the algorithm.
*
*	@returns true if the hole could be filled, otherwise false
*/

bool fill_hole(	int num_vertices, long* vertex_IDs, double* coordinates,
		int* num_new_vertices, double** new_coordinates, int* num_new_faces, long** new_vertex_IDs)
{
	bool result = true;
	if(	num_vertices == 0		||
		vertex_IDs == NULL		||
		coordinates == NULL		||
		num_new_vertices == NULL	||
		new_coordinates == NULL		||
		num_new_faces == NULL		||
		new_vertex_IDs == NULL)
		return(false);

	// Create formatted input data for the hole

	std::vector< std::pair<v3ctor, size_t> > vertices;
	for(int i = 0; i < num_vertices; i++)
	{
		v3ctor vertex_position;

		vertex_position[0] = coordinates[3*i];
		vertex_position[1] = coordinates[3*i+1];
		vertex_position[2] = coordinates[3*i+2];

		vertices.push_back(std::make_pair(vertex_position, vertex_IDs[i]));
	}

	psalm::hole H;
	H.initialize(vertices);
	H.triangulate();
	try
	{
		H.subdivide(psalm::mesh::ALG_LIEPA);
		H.save_raw_data(num_new_vertices, new_coordinates, num_new_faces, new_vertex_IDs);
	}

	// TODO: This should be handled more gracefully
	catch(...)
	{
		result = false;
	}

	return(result);
}
