/*!
*	@file 	t_edge_hash.cpp
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
*	@param 	e Edge to add
*/

edge_query t_edge_hash::add(const edge& e)
{
	edge_query result;

	// Calculate ID of edge. This maps the lower half of N^2 to N.
	size_t n = 178975;
	size_t k;
	if(e.u >= e.v)
		k = (e.v+1)*n+(e.u+1)-n;
	else
		k = (e.u+1)*n+(e.v+1)-n;

	// Check whether edge exists
	std::hash_map<size_t, size_t>::iterator it;
	if((it = T.find(k)) == T.end())
	{
		// Edge not found, add the _original_ (!) edge to the
		// vector
		E.push_back(e);
		T[k] = E.size()-1;

		result.e = E.size()-1;
		result.inverted = false;
	}
	else
	{
		// Edge has been found, check whether the proper direction has
		// been stored 
		if(E[it->second].u != e.u)
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

edge& t_edge_hash::get(size_t e)
{
	// TODO: Handle invalid ranges
	return(E[e]);
}
