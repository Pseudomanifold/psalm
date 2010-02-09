/*!
*	@file 	edge.cpp
*	@brief 	Functions and implementations for edge class
*/

#include "edge.h"

/*!
*	Default constructor. Creates invalid edge by setting both parameters to
*	0.
*/

edge::edge()
{
	u = v = 0;
}

/*!
*	Constructor that creates a new edge (u,v).
*
*	@param 	u Start vertex index
*	@param 	v End vertex index
*/

edge::edge(size_t u, size_t v)
{
	this->u = u;
	this->v = v;
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

bool edge::operator==(const edge& b) const
{
	if(	u == b.u && v == b.v ||
		v == b.u && u == b.v)
		return(true);
	else
		return(false);
}
