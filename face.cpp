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
