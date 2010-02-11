/*!
*	@file	vertex.cpp
*	@brief	Implementations for vertex class
*/

#include "vertex.h"

/*!
*	Adds incident edge to vertex.
*/

void vertex::add_incident_edge(const size_t& e)
{
	E.push_back(e);
}

/*!
*	Returns index of incident edge.
*/

size_t& vertex::get(const size_t& i)
{
	// TODO: Check invalid range?
	return(E[i]);
}

/*!
*	Returns number of incident edges, i.e., the valence.
*/

size_t vertex::size()
{
	return(E.size());
}
