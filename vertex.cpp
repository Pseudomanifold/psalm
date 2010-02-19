/*!
*	@file	vertex.cpp
*	@brief	Implementations for vertex class
*/

#include "vertex.h"

/*!
*	Default constructor for vertex.
*/

vertex::vertex()
{
	// TODO:
	// Fill with interesting code?
}

/*!
*	Constructor that sets all vertex attributes at once.
*/

vertex::vertex(double x, double y, double z, size_t id)
{
	set(x,y,z,id);
}

/*!
*	Sets vertex data.
*
*	@param x	x value of vertex position
*	@param y	y value of vertex position
*	@param z	z value of vertex position
*	@param id	ID of vertex
*/

void vertex::set(double x, double y, double z, size_t id)
{
	this->p[0]	= x;
	this->p[1]	= y;
	this->p[2]	= z;
	this->id	= id;
}

/*!
*	Returns reference to vertex position.
*/

const v3ctor& vertex::get_position() const
{
	return(p);
}

/*!
*	Returns vertex ID.
*/

size_t vertex::get_id() const
{
	return(id);
}

/*!
*	Adds incident edge to vertex.
*/

void vertex::add_edge(const edge* e)
{
	E.push_back(e);
}

/*
*	Returns pointer to incident edge.
*/

const edge* vertex::get_edge(size_t i)
{
	// TODO: Check invalid range?
	return(E[i]);
}

/*!
*	Returns the valency of the vertex, i.e., the number of incident edges.
*/

size_t vertex::valency() const
{
	return(E.size());
}

/*!
*	Adds an adjacent face to the vertex.
*/

void vertex::add_face(const face* f)
{
	F.push_back(f);
}

/*!
*	Returns an adjacent face of the vertex.
*/

const face* vertex::get_face(size_t i)
{
	// FIXME: Check range?
	return(F[i]);
}

/*!
*	Returns the number of adjacent faces for the current vertex.
*/

size_t vertex::num_adjacent_faces() const
{
	return(F.size());
}
