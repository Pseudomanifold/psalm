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
*	Sets the provided parameters to the current start and end vertex of the
*	edge.
*
*	@param u Will contain reference to start vertex
*	@param v Will contain reference to end vertex
*/

void edge::get(vertex* u, vertex* v)
{
	u = this->u;
	v = this->v;
}


/*!
*	Comparison operator for edges: Weak ordering by index is applied,
*	hence (u,v) < (u',v') if u < u' or (u == u' and v < v').
*
*	@param	b Edge to compare current edge with.
*	@return	true if current edge is smaller than b, else false.
*/

bool edge::operator<(const edge& b) const
{
	if(u < b.u)
		return(true);
	else if(u == b.u)
		return(v < b.v);

	return(false);
}
