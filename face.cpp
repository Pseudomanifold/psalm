/*!
*	@file	face.cpp
*	@brief	Functions for mesh faces
*/

#include <set>
#include <iostream>
#include <stdexcept>
#include <limits>

#include "face.h"
#include "edge.h"

namespace psalm
{

/*!
*	Default constructor for the face.
*/

face::face()
{
	face_point	= NULL;
	id		= std::numeric_limits<size_t>::max();
	boundary	= false;
	obtuse		= boost::logic::indeterminate;
}

/*!
*	Adds an edge to the face.
*
*	@param result Directed edge to add to face
*
*	@warning Edge is not checked for consistency, i.e., you can add edges
*	that are not part of the face.
*/

void face::add_edge(const directed_edge& result)
{
	E.push_back(result);
}

/*!
*	@param	i Index of edge
*	@return Edge at specified index
*/

directed_edge& face::get_edge(size_t i)
{
	return(const_cast<directed_edge&>(static_cast<const face*>(this)->get_edge(i)));
}

/*!
*	@param	i Index of edge
*	@return	Const reference to edge at specified index
*/

const directed_edge& face::get_edge(size_t i) const
{
	if(i >= E.size())
		throw std::out_of_range("face::get_edge(): Index out of range");
	else
		return(E[i]);
}

/*!
*	Adds vertex to the face.
*
*	@param v Pointer to new vertex.
*	@warning The vertex is not checked for consistency, i.e., you can add
*	any vertex to the face (even vertices that are not part of the face at
*	all).
*/

void face::add_vertex(vertex* v)
{
	V.push_back(v);
}

/*!
*	@param	i Index of vertex
*	@return	Vertex at specified index
*	@throws	std::out_of_range if the index is out of range
*/

vertex* face::get_vertex(size_t i)
{
	return(const_cast<vertex*>(static_cast<const face*>(this)->get_vertex(i)));
}

/*!
*	@param i Index of vertex
*	@return Vertex at specified index or NULL if the index is out of range.
*	@throws	std::out_of_range if the index is out of range
*/

const vertex* face::get_vertex(size_t i) const
{
	if(i >= V.size())
		throw(std::out_of_range("face::get_vertex(): Index out of range"));
	else
		return(V[i]);
}

/*!
*	@return Number of vertices for face, i.e., whether the face is a
*	triangle, a quadrangle, ...
*/

size_t face::num_vertices() const
{
	return(V.size());
}

/*!
*	@return Number of edges for face
*/

size_t face::num_edges() const
{
	return(E.size());
}

/*!
*	Stores a new face vertex that corresponds to a vertex in the mesh.
*
*	@param v Pointer to new vertex.
*	@warning The new vertex cannot be checked for consistency, i.e., there
*	is no way of finding out whether the new vertex corresponds to a vertex
*	of the face.
*/

void face::add_face_vertex(vertex* v)
{
	V_F.push_back(v);
}

/*!
*	@param i Index of face vertex.
*	@return Face vertex that corresponds to a vertex in the mesh or NULL if
*	the index is out of range.
*/

vertex* face::get_face_vertex(size_t i)
{
	if(i >= V_F.size())
		return(NULL);
	else
		return(V_F[i]);
}

/*!
*	Returns value of flag signalling whether the face is a boundary face.
*	Value of flag is supposed to be set by the user.
*/

bool face::is_on_boundary() const
{
	return(boundary);
}

/*!
*	Sets value of flag signalling boundary faces. The parameter is set to
*	false by default in all constructors.
*
*	@param	boundary Current value for boundary parameter (true by default)
*/

void face::set_on_boundary(bool boundary)
{
	this->boundary = boundary;
}

/*!
*	Calculates the area of the face. Currently, this only works if the face
*	is triangular.
*
*	@return Unsigned area of the face; negative values indicate an error
*/

double face::calc_area() const
{
	if(this->num_vertices() != 3)
		return(-1.0);

	v3ctor A = V[1]->get_position() - V[0]->get_position();
	v3ctor B = V[2]->get_position() - V[0]->get_position();

	return(0.5*(A|B).length());
}

/*!
*	Checks wether the face is an obtuse triangle. This requires evaluating
*	the `is_obtuse` flag. If the value of this flag has not yet been set,
*	the function performs an obtusity test, sets the flag, and returns its
*	value.
*
*	@return true if the face is an obtuse triangle, else false
*/

bool face::is_obtuse()
{
	if(boost::logic::indeterminate(obtuse))
	{
		double a = E[0].e->calc_length();
		double b = E[1].e->calc_length();
		double c = E[2].e->calc_length();

		obtuse = (a*a+b*b < c*c) || (b*b* + c*c < a*a) || (c*c + a*a < b*b);
	}

	return(bool(obtuse) == true);
}

} // end of namespace "psalm"
