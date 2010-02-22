/*!
*	@file	face.cpp
*	@brief	Functions for mesh faces
*/

#include "face.h"

/*!
*	Adds an edge to the face.
*
*	@param result Directed edge to add to face
*/

void face::add_edge(const directed_edge& result)
{
	// TODO: Check whether indices are valid
	E.push_back(result);
}

/*!
*	Returns edge with a certain index.
*/

directed_edge& face::get_edge(size_t i)
{
	// TODO:
	// Check for invalid ranges
	return(E[i]);
}

/*!
*	Adds vertex to the face.
*/

void face::add_vertex(vertex* v)
{
	V.push_back(v);
}

/*!
*	Returns vertex with a certain index.
*/

const vertex* face::get_vertex(size_t i) const
{
	// TODO:
	// Check for invalid ranges
	return(V[i]);
}


/*!
*	Finds specified edge and returns edge_query structure (so that the
*	orientation is known).
*/

directed_edge* face::find_edge(const size_t& e, size_t& pos)
{
	/*
	for(size_t i = 0; i < E.size(); i++)
	{
		if(E[i].e == e)
		{
			pos = i;
			return(E[i]);
		}
	}

	return(E[0]);
	*/

	return(NULL);
}

/*!
*	Returns number of vertices for face, i.e., whether the face is a
*	triangle, a quadrangle, ...
*/

size_t face::num_vertices() const
{
	return(V.size());
}

/*!
*	Returns number of edges for face.
*/

size_t face::num_edges() const
{
	return(E.size());
}

/*!
*	Stores a new face vertex that corresponds to a vertex in the mesh.
*/

void face::add_face_vertex(vertex* v)
{
	V_F.push_back(v);
}

/*!
*	Returns a face vertex that corresponds to a vertex in the mesh.
*/

const vertex* face::get_face_vertex(size_t i) const
{
	// FIXME: Check range? Check ID?
	return(V_F[i]);
}

/*!
*	Sets ID of the face.
*/

void face::set_id(size_t id)
{
	this->id = id;
}

/*!
*	Returns face ID.
*/

size_t face::get_id() const
{
	return(id);
}
