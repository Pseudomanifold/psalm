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
