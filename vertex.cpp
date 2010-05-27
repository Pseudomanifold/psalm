/*!
*	@file	vertex.cpp
*	@brief	Implementations for vertex class
*/

#include "vertex.h"

namespace psalm
{

/*!
*	Default constructor for vertex.
*/

vertex::vertex()
{
}

/*!
*	Constructor that sets all relevant vertex attributes at once.
*
*	@param x	x value of vertex position
*	@param y	y value of vertex position
*	@param z	z value of vertex position
*	@param id	ID of vertex
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
*	@return Const reference to vertex position.
*/

const v3ctor& vertex::get_position() const
{
	return(p);
}

/*!
*	@return ID of the vertex
*/

size_t vertex::get_id() const
{
	return(id);
}

/*!
*	Adds incident edge to vertex.
*
*	@param e Pointer to incident edge.
*	@warning The edge is not checked for consistency.
*/

void vertex::add_edge(edge* e)
{
	E.push_back(e);
}

/*!
*	@param i Edge index
*	@return Pointer to incident edge or NULL if the index is out of bounds.
*/

edge* vertex::get_edge(size_t i)
{
	if(i >= E.size())
		return(NULL);
	else
		return(E[i]);
}

/*!
*	@return Valency of the vertex, i.e., the number of incident edges.
*/

size_t vertex::valency() const
{
	return(E.size());
}

/*!
*	Adds an adjacent face to the vertex.
*
*	@param f Pointer to adjacent face.
*	@warning The face is not checked for consistency.
*/

void vertex::add_face(const face* f)
{
	F.push_back(f);
}

/*!
*	@param i Index of adjacent face
*
*	@return Pointer to an adjacent face of the vertex or NULL if the index
*	is out of bounds.
*/

const face* vertex::get_face(size_t i) const
{
	if(i >= F.size())
		return(NULL);
	else
		return(F[i]);
}

/*!
*	@return Number of adjacent faces for the current vertex.
*/

size_t vertex::num_adjacent_faces() const
{
	return(F.size());
}

} // end of namespace "psalm"
