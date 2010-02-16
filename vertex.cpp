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

const v3ctor& vertex::get_position()
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

void vertex::add_incident_edge(const edge* e)
{
	E.push_back(e);
}

/*!
*	Returns index of incident edge.
*/

//size_t& vertex::get(const size_t& i)
//{
//	// TODO: Check invalid range?
//	return(E[i]);
//}

/*!
*	Returns number of incident edges, i.e., the valence.
*/

size_t vertex::size() const
{
	return(E.size());
}
