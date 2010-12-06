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
*	Reconstructs the face from its edges:
*
*	- Delete vertices
*	- Traverse edges
*	- Store vertices in the order specified by the edges
*/

void face::reconstruct_from_edges()
{
	V.clear();
	std::cout << "HAVE: ";
	for(std::vector<directed_edge>::iterator e_it = E.begin(); e_it < E.end(); e_it++)
	{
		if(e_it->inverted)
			std::cout << "* ";

		std::cout << e_it->e->get_u()->get_id() << " ";
		std::cout << e_it->e->get_v()->get_id() << " ";

		// Only store the first vertex of an edge -- this will yield
		// _all_ vertices upon traversal
		if(e_it->inverted)
			add_vertex(const_cast<vertex*>(e_it->e->get_v())); // XXX: Evil. face::add_vertex() should be fixed
		else
			add_vertex(const_cast<vertex*>(e_it->e->get_u()));
	}
	std::cout << "\n";

	std::set<size_t> V_IDs;

	std::cout << "\tRECONSTRUCTED: ";
	for(size_t i = 0; i < V.size(); i++)
	{
		std::cout << V[i]->get_id() << " ";
		V_IDs.insert(V[i]->get_id());
	}

	std::cout << " [ SORTED: ";
	for(std::set<size_t>::iterator it = V_IDs.begin(); it != V_IDs.end(); it++)
	{
		std::cout << *it << " ";
	}
	std::cout << "] " << "\n";

	if(V_IDs.size() < 3)
		std::cout << "DISASTER.\n";

}

} // end of namespace "psalm"
