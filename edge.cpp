/*!
*	@file	edge.cpp
*	@brief	Functions and implementations for edge class
*/

#include <cassert>
#include "edge.h"

/*!
*	Default constructor that creates and invalid edge.
*/

edge::edge()
{
	u = v = NULL;
	f = g = NULL;
}

/*!
*	Constructor that creates a new edge (u,v).
*
*	@param	u Pointer to start vertex
*	@param	v Pointer to end vertex
*/

edge::edge(const vertex* u, const vertex* v)
{
	set(u,v);
}

/*!
*	Sets start and end vertex of the edge.
*
*	@param	u Pointer to start vertex
*	@param	v Pointer to end vertex
*/

void edge::set(const vertex* u, const vertex* v)
{
	if(u == v)
		u = v = NULL;

	this->u = u;
	this->v = v;
}

/*!
*	Returns pointer to start vertex.
*/

const vertex* edge::get_u() const
{
	return(u);
}

/*!
*	Returns pointer to end vertex.
*/

const vertex* edge::get_v() const
{
	return(v);
}

/*!
*	Sets pointer to first adjacent face.
*/

void edge::set_f(const face* f)
{
	assert(g == NULL && this->f == NULL);
	this->f = f;
}

/*!
*	Sets pointer to second adjacent face.
*/

void edge::set_g(const face* g)
{
	assert(f != NULL && this->g == NULL);
	this->g = g;
}

/*!
*	Returns pointer to first adjacent face.
*/

const face* edge::get_f() const
{
	return(f);
}

/*!
*	Returns pointer to second adjacent face.
*/

const face* edge::get_g() const
{
	return(g);
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
