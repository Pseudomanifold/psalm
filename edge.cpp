/*!
*	@file	edge.cpp
*	@brief	Functions and implementations for edge class
*/

#include <iostream>
#include <cmath>

#include "edge.h"

namespace psalm
{

/*!
*	Default constructor that creates and invalid edge.
*/

edge::edge()
{
	set(NULL, NULL);
}

/*!
*	Constructor that creates a new edge (u,v).
*
*	@param u Pointer to start vertex
*	@param v Pointer to end vertex
*/

edge::edge(vertex* u, vertex* v)
{
	set(u,v);
}

/*!
*	Sets start and end vertex of the edge.
*
*	@param u Pointer to start vertex
*	@param v Pointer to end vertex
*/

void edge::set(vertex* u, vertex* v)
{
	this->u = u;
	this->v = v;

	if(u == v)
		this->u = this->v = NULL;

	f = g		= NULL;
	edge_point	= NULL;
	boundary	= boost::logic::indeterminate;
}

/*!
*	@return Pointer to start vertex
*/

vertex* edge::get_u()
{
	return(const_cast<vertex*>(static_cast<const edge*>(this)->get_u()));
}

/*!
*	@return Const pointer to start vertex
*/

const vertex* edge::get_u() const
{
	return(u);
}

/*!
*	Sets new start vertex. Does not alter remaining attributes.
*
*	@param Pointer to new start vertex
*/

void edge::set_u(vertex* u)
{
	this->u = u;
}

/*!
*	@return Pointer to end vertex
*/

vertex* edge::get_v()
{
	return(const_cast<vertex*>(static_cast<const edge*>(this)->get_v()));
}

/*!
*	@return Const pointer to end vertex
*/

const vertex* edge::get_v() const
{
	return(v);
}

/*!
*	Sets new end vertex. Does not alter remaining attributes.
*
*	@param Pointer to new end vertex
*/

void edge::set_v(vertex* v)
{
	this->v = v;
}

/*!
*	Sets pointer to first adjacent face.
*
*	@param f Pointer to face
*
*	@warning The function does not check if the current edge is really a
*	part of the face.
*/

void edge::set_f(face* f)
{
	this->f = f;
}

/*!
*	Sets pointer to second adjacent face.
*
*	@param g Pointer to face
*
*	@warning The function does not check if the current edge is really a
*	part of the face.
*/

void edge::set_g(face* g)
{
	static bool warning_shown = false;
	if(	f != NULL && this->g != NULL &&
		g != NULL) // warning is not shown if the second face is _reset_
	{
		if(!warning_shown)
		{
			std::cerr << "psalm: Warning: Mesh might be non-manifold.\n";
			warning_shown = true;
		}

		return;
	}

	this->g = g;
}

/*!
*	@return Pointer to first adjacent face.
*/

face* edge::get_f()
{
	return(const_cast<face*>(static_cast<const edge*>(this)->get_f()));
}

/*!
*	@return Constant pointer to first adjacent face
*/

const face* edge::get_f() const
{
	return(f);
}

/*!
*	@return Pointer to second adjacent face
*/

face* edge::get_g()
{
	return(const_cast<face*>(static_cast<const edge*>(this)->get_g()));
}

/*!
*	@return Constant pointer to second adjacent face
*/

const face* edge::get_g() const
{
	return(g);
}

/*!
*	Returns value of flag signalling whether the edge is a boundary edge.
*	This flag may be set by the user, but the edge is also able to
*	determine wether it is a boundary edge by taking a look at adjacent
*	faces.
*
*	The value of the flag is calculated if the boundary flag is set to be
*	indeterminate.
*
*	@return true if the edge is a boundary edge, else false
*/

bool edge::is_on_boundary()
{
	if(boost::logic::indeterminate(boundary))
		boundary = (f == NULL) || (g == NULL);

	return(bool(boundary) == true);
}

/*!
*	Sets value of flag signalling boundary edges. By default, the value of
*	the flag is indeterminate.
*
*	@param	boundary Current value for boundary parameter (true by default)
*/

void edge::set_on_boundary(bool boundary)
{
	this->boundary = boundary;
}

/*!
*	Given another edge that is assumed to share one vertex with the current
*	edge, calculate the interior angle between these two edges.
*
*	@warning The result is undefined if both edges do _not_ share a common
*	vertex.
*
*	@return Interior angle between edge e and current edge
*/

double edge::calc_angle(const edge* e) const
{
	// Check that both edges point into the _same_ direction -- otherwise
	// the wrong angle would be calculated.

	if(	this->get_u() == e->get_u() ||
		this->get_v() == e->get_v())
	{
		v3ctor a = this->get_u()->get_position() - this->get_v()->get_position();
		v3ctor b = e->get_u()->get_position() - e->get_v()->get_position();

		return(acos(a.normalize()*b.normalize()));
	}
	else
	{
		v3ctor a = this->get_u()->get_position() - this->get_v()->get_position();
		v3ctor b = e->get_v()->get_position() - e->get_u()->get_position(); // swap second edge

		return(acos(a.normalize()*b.normalize()));
	}
}

} // end of namespace "psalm"
