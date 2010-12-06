/*!
*	@file	vertex.cpp
*	@brief	Implementations for vertex class
*/

#include <limits>
#include "vertex.h"
#include "edge.h"

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
*	@param	i Edge index
*	@return Pointer to incident edge
*	@throws	std::out_of_range if the index is out of range
*/

edge* vertex::get_edge(size_t i)
{
	return(const_cast<edge*>(static_cast<const vertex*>(this)->get_edge(i)));
}

/*!
*	@param i Edge index
*	@return Constant pointer to incident edge
*	@throws	std::out_of_range if the index is out of range
*/

const edge* vertex::get_edge(size_t i) const
{
	if(i >= E.size())
		throw(std::out_of_range("vertex::get_edge(): Index out of range"));
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
*	@param	i Index of adjacent face
*	@return Pointer to an adjacent face of the vertex
*	@throws	std::out_of_range if the index is out of bounds
*/

const face* vertex::get_face(size_t i) const
{
	if(i >= F.size())
		throw(std::out_of_range("vertex::get_face(): Index out of range"));
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

/*!
*	Calculates discrete Laplacian operator as the vertex position
*	subtracted from the average of adjacent vertices.
*/

v3ctor vertex::discrete_laplacian() const
{
	v3ctor res;
	v3ctor average;

	size_t n = this->valency();
	for(size_t i = 0; i < n; i++)
	{
		const edge* e = get_edge(i);
		if(e->get_u() == const_cast<const vertex*>(this))
			average += this->get_edge(i)->get_v()->get_position()/static_cast<double>(n);
		else
			average += this->get_edge(i)->get_u()->get_position()/static_cast<double>(n);
	}

	res = average-this->get_position();
	return(res);
}

/*!
*	Calculates discrete bilaplacian operator as the discrete Laplacian of
*	the vertex position subtracted from the average of the discrete
*	Laplacian of adjacent vertices.
*/

v3ctor vertex::discrete_bilaplacian() const
{
	v3ctor res;
	v3ctor average;

	size_t n = this->valency();
	for(size_t i = 0; i < n; i++)
	{
		const edge* e = get_edge(i);
		if(e->get_u() == const_cast<const vertex*>(this))
			average += this->get_edge(i)->get_v()->discrete_laplacian()/static_cast<double>(n);
		else
			average += this->get_edge(i)->get_u()->discrete_laplacian()/static_cast<double>(n);
	}

	res = average-this->discrete_laplacian();
	return(res);
}

} // end of namespace "psalm"
