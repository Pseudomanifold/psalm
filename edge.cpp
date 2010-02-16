/*!
*	@file	edge.cpp
*	@brief	Functions and implementations for edge class
*/

#include "edge.h"

/*!
*	Default constructor that creates and invalid edge.
*/

edge::edge()
{
	u = v = NULL;
}

/*!
*	Constructor that creates a new edge (u,v).
*
*	@param	u Pointer to start vertex
*	@param	v Pointer to end vertex
*/

edge::edge(vertex* u, vertex* v)
{
	this->u = u;
	this->v = v;
}

/*!
*	Sets start and end vertex of the edge.
*
*	@param	u Pointer to start vertex
*	@param	v Pointer to end vertex
*/

void edge::set(vertex* u, vertex* v)
{
	if(u == v)
		u = v = NULL;

	this->u = u;
	this->v = v;
}

/*!
*	Returns pointer to start vertex.
*/

vertex* edge::get_u()
{
	return(u);
}

/*!
*	Returns pointer to end vertex.
*/

vertex* edge::get_v()
{
	return(v);
}


/*!
*	Comparison operator for edges: Weak ordering by index of the vertices
*	is applied, hence (u,v) < (u',v') if u < u' or (u == u' and v < v').
*
*	@param	b Edge to compare current edge with.
*	@return	true if current edge is smaller than b, else false.
*/

bool edge::operator<(const edge& b) const
{
	if(u->get_id() < b.u->get_id())
		return(true);
	else if(u->get_id() == b.u->get_id())
		return(v->get_id() < b.v->get_id());

	return(false);
}
