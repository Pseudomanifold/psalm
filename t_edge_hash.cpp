/*!
*	@file	t_edge_hash.cpp
*	@brief	Implementation of an edge lookup table using a hash map.
*/

#include "t_edge_hash.h"

/*!
*	Returns current number of stored edges.
*/

size_t t_edge_hash::size()
{
	return(E.size());
}

/*!
*	Adds an edge to the edge table if it (or the inverted edge) does not
*	already exist.
*
*	@param	e Edge to add
*/

directed_edge t_edge_hash::add(edge& e)
{
	directed_edge result;

	// Calculate ID of edge. This maps the lower half of N^2 to N.
//	size_t n = 178975;
//	size_t k;
//	if(e.u >= e.v)
//		k = (e.v+1)*n+(e.u+1)-n;
//	else
//		k = (e.u+1)*n+(e.v+1)-n;

	// FIXME
	size_t k = 1;

	// Check whether edge exists
	std::hash_map<size_t, edge*>::iterator it;
	if((it = T.find(k)) == T.end())
	{
		// Edge not found, create an edge from the _original_ edge and
		// add it to the map
		edge* new_edge = new edge(e.get_u(), e.get_v());
		E.push_back(new_edge);
		T[k] = new_edge;

		result.e = new_edge;
		result.inverted = false;
	}
	else
	{
		// Edge has been found, check whether the proper direction has
		// been stored.
		vertex* u1 = it->second->get_u();
		vertex* u2 = e.get_u();
		if(u1 != u2)
			result.inverted = true;
		else
			result.inverted = false;

		result.e = it->second;
	}

	return(result);
}

/*!
*	Returns edge from edge vector.
*/

edge* t_edge_hash::get(size_t e)
{
	// TODO: Handle invalid ranges
	return(E[e]);
}
