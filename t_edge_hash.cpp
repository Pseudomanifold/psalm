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

	/*
		Calculate ID of edge by using the Cantor pairing function. If
		necessary, the IDs of the edge's vertices are swapped so that
		k1 will always be the less or equal to k2. This is done in
		order to provide a natural sorting order for the edges.
	*/

	size_t k1, k2;
	if(e.get_u()->get_id() < e.get_v()->get_id())
	{
		k1 = e.get_u()->get_id();
		k2 = e.get_v()->get_id();
	}
	else
	{
		k1 = e.get_v()->get_id();
		k2 = e.get_u()->get_id();
	}

	size_t k = static_cast<size_t>(0.5*(k1+k2)*(k1+k2+1)+k2);

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
		result.new_edge = true;
	}
	else
	{
		// Edge has been found, check whether the proper direction has
		// been stored.
		if(it->second->get_u()->get_id() != e.get_u()->get_id())
			result.inverted = true;
		else
			result.inverted = false;

		result.new_edge = false;
		result.e = it->second;
	}

	return(result);
}

/*!
*	Returns edge from edge vector.
*/

edge* t_edge_hash::get(size_t e)
{
	if(e >= E.size())
		return(NULL);
	else
		return(E[e]);
}

/*!
*	Destroys the edge table by clearing all internal data structures.
*	Memory is freed by default, although this behaviour can be toggled.
*
*	@param free_memory Toggles whether memory is freed (by default) or not.
*/

void t_edge_hash::destroy(bool free_memory)
{
	if(free_memory)
	{
		for(std::vector<edge*>::iterator it = E.begin(); it != E.end(); it++)
		{
			if(*it != NULL)
				delete(*it);
		}
	}

	E.clear();
	T.clear();
}
