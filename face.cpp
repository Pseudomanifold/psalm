/*!
*	@file	face.cpp
*	@brief	Functions for mesh faces
*/

#include <stdexcept>
#include "face.h"

namespace psalm
{

/*!
*	Default constructor for the face.
*/

face::face()
{
	face_point = NULL;
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
	if(i >= E.size())
		throw std::out_of_range("face::get_edge(): Invalid edge index");
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
*	@param i Index of vertex
*	@return Vertex at specified index or NULL if the index is out of range.
*/

const vertex* face::get_vertex(size_t i) const
{
	if(i >= V.size())
		return(NULL);
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

} // end of namespace "psalm"
