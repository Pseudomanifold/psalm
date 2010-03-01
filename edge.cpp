/*!
*	@file	edge.cpp
*	@brief	Functions and implementations for edge class
*/

#include <iostream>
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
*	@param u Pointer to start vertex
*	@param v Pointer to end vertex
*/

edge::edge(const vertex* u, const vertex* v)
{
	set(u,v);
}

/*!
*	Sets start and end vertex of the edge.
*
*	@param u Pointer to start vertex
*	@param v Pointer to end vertex
*/

void edge::set(const vertex* u, const vertex* v)
{
	if(u == v)
		u = v = NULL;

	this->u = u;
	this->v = v;

	f = g = NULL;
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
*
*	@param f Pointer to face
*	@warning The function does not check if the current edge is really a
*	part of the face.
*/

void edge::set_f(face* f)
{
	// FIXME
//	std::cout << "set_f(" << u->get_id() << "," << v->get_id() << ")\n";
//	assert(g == NULL && this->f == NULL);
//	this->f = f;
	if(this->f == NULL)
		this->f = f;
	else if(this->g == NULL)
	{
		std::cerr	<< "["	<< u->get_id() << ","
					<< v->get_id() << "].set_f: Face f already set, setting face g.\n";

		this->g = f;
	}
	else
	{
		std::cerr	<< "["	<< u->get_id() << ","
					<< v->get_id() << "].set_f: Both faces already set. Don't know what to do.\n";
	}
}

/*!
*	Sets pointer to second adjacent face.
*
*	@param g Pointer to face
*	@warning The function does not check if the current edge is really a
*	part of the face.
*/

void edge::set_g(face* g)
{
	// FIXME
//	std::cout << "set_g(" << u->get_id() << "," << v->get_id() << ")\n";
//	assert(f != NULL && this->g == NULL);
//	this->g = g;
	if(this->g == NULL)
		this->g = g;
	else if(this->f == NULL)
	{
		std::cerr	<< "["	<< u->get_id() << ","
					<< v->get_id() << "].set_g: Face g already set, setting face f.\n";

		this->f = g;
	}
	else
	{
		std::cerr	<< "["	<< u->get_id() << ","
					<< v->get_id() << "].set_g: Both faces already set. Don't know what to do.\n";
	}
}

/*!
*	Returns pointer to first adjacent face.
*/

face* edge::get_f()
{
	return(f);
}

const face* edge::get_f() const
{
	return(f);
}

/*!
*	Returns pointer to second adjacent face.
*/

face* edge::get_g()
{
	return(g);
}

const face* edge::get_g() const
{
	return(g);
}
