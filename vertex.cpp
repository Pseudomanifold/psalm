/*!
*	@file	vertex.cpp
*	@brief	Implementations for vertex class
*/

#include <limits>
#include "vertex.h"

namespace psalm
{

/*!
*	Default constructor for vertex.
*/

vertex::vertex()
{
	boundary	= false;
	vertex_point	= NULL;
	id		= std::numeric_limits<size_t>::max();
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

	// Needs to be initialized, otherwise it cannot be determined whether a
	// vertex already has a vertex point
	vertex_point = NULL;

	// By default, no vertex is a boundary vertex. This attribute only
	// becomes relevant if boundary vertices are to be preserved.
	boundary = false;
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
*	@param i Edge index
*	@return Constant pointer to incident edge or NULL if the index is out
*		of bounds.
*/

const edge* vertex::get_edge(size_t i) const
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

/*!
*	Returns value of flag signalling whether the vertex is a boundary
*	vertex.
*/

bool vertex::is_on_boundary() const
{
	return(boundary);
}

/*!
*	Sets value of flag signalling boundary vertices. The parameter is set
*	to false by default in all constructors.
*
*	@param	boundary Current value for boundary parameter (true by default)
*/

void vertex::set_on_boundary(bool boundary)
{
	this->boundary = boundary;
}

} // end of namespace "psalm"
