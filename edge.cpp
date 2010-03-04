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
*	@return Pointer to start vertex
*/

const vertex* edge::get_u() const
{
	return(u);
}

/*!
*	@return Pointer to end vertex
*/

const vertex* edge::get_v() const
{
	return(v);
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
	assert(g == NULL && this->f == NULL);
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
	assert(f != NULL && this->g == NULL);
	this->g = g;
}

/*!
*	@return Pointer to first adjacent face.
*/

face* edge::get_f()
{
	return(f);
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
	return(g);
}

/*!
*	@return Constant pointer to second adjacent face
*/

const face* edge::get_g() const
{
	return(g);
}
