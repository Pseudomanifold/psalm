/*!
*	@file 	face.cpp
*	@brief	Functions for mesh faces
*/

#include "face.h"

/*!
*	Adds an edge to the face.
*
*	@param result Directed edge to add to face
*/

void face::add_edge(const edge_query& result)
{
	// TODO: Check whether indices are valid
	E.push_back(result);
}

/*!
*	Finds specified edge and returns edge_query structure (so that the
*	orientation is known).
*/

edge_query& face::find_edge(const size_t& e, size_t& pos)
{
	for(size_t i = 0; i < E.size(); i++)
	{
		if(E[i].e == e)
		{
			pos = i;
			return(E[i]);
		}
	}

	return(E[0]);
}
