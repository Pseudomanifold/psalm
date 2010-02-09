/*!
*	@file	t_edge.cpp
*	@brief	Functions for edge lookup table
*/

#include "t_edge.h"

/*!
*	Returns current number of stored edges.
*/

size_t t_edge::size()
{
	return(E.size());
}

/*!
*	Adds an edge to the edge table if it (or the inverted edge) does not
*	already exist.
*
*	@param 	e Edge to add
*/

edge_query t_edge::add(const edge& e)
{
	// TODO: Need to think about whether it makes sense _NOT_ to check for
	// the inverted edge.

	edge_query result;

	// Check whether edge exists
	std::map<edge, size_t>::iterator it;
	if((it = T.find(e)) == T.end())
	{
		// Check inverted edge
		edge e_(e.v, e.u);
		if((it = T.find(e_)) == T.end())
		{
			// Edge not found, add the _original_ (!) edge to the
			// vector
			E.push_back(e);
			T[e] = E.size()-1;

			result.e = E.size()-1;
			result.inverted = false;
		}
		else
		{
			// Inverted edge found
			result.e = it->second;
			result.inverted = true;
		}
	}
	else
	{
		// Original edge found
		result.e = it->second;
		result.inverted = false;
	}

	return(result);
}

/*!
*	Returns edge from edge vector.
*/

edge& t_edge::get(size_t e)
{
	// TODO: Handle invalid ranges
	return(E[e]);
}
