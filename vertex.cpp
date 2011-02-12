/*!
*	@file	vertex.cpp
*	@brief	Implementations for vertex class
*/

#include <iostream>
#include <limits>
#include <cmath>

#include "vertex.h"
#include "edge.h"

namespace psalm
{

/*!
*	Sets default values for a vertex.
*/

vertex::vertex()
{
	boundary	= false;
	vertex_point	= NULL;
	id		= std::numeric_limits<size_t>::max();
	scale_attribute	= 0.0;
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
	set(x, y, z, 0.0, 0.0, 0.0, id);
}

vertex::vertex(double x, double y, double z, double nx, double ny, double nz, size_t id)
{
	set(x, y, z, nx, ny, nz, id);
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
	set(	x,
		y,
		z,
		0.0, // default normal vector
		0.0,
		0.0,
		id);
}

void vertex::set(double x, double y, double z, double nx, double ny, double nz, size_t id)
{
	this->p[0]	= x;
	this->p[1]	= y;
	this->p[2]	= z;
	this->n[0]	= nx;
	this->n[1]	= ny;
	this->n[2]	= nz;
	this->id	= id;

	// Needs to be initialized, otherwise it cannot be determined whether a
	// vertex already has a vertex point
	vertex_point = NULL;

	// By default, no vertex is a boundary vertex. This attribute only
	// becomes relevant if boundary vertices are to be preserved.
	boundary = false;

	// Sensible default for any vertex. Negative values make no sense, as
	// the scale attribute is composed of edge lengths.
	scale_attribute = 0.0;
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
*	Removes an adjacent face from the list of adjacent faces of the
*	vertex.
*
*	@param f Pointer to adjacent face
*
*	@warning This function does _not_ free any memory used by the face.
*	Only its reference in the list of adjacent faces is removed.
*/

void vertex::remove_face(const face* f)
{
	std::vector<const face*>::iterator face_pos = std::find(F.begin(), F.end(), f);
	if(face_pos == F.end())
		throw(std::runtime_error("vertex::remove_face(): Unable to find face in face vector"));
	else
		F.erase(face_pos);
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

/*!
*	Enumerates all neighbours of the current vertex and returns them in a
*	vector. The order of the enumeration is not fixed.
*
*	@returns Vector of neighbouring vertices, i.e. the 1-ring neighbourhood
*	of the vertex
*/

std::vector<const vertex*> vertex::get_neighbours() const
{
	std::vector<const vertex*> res;

	// We enumerate the neighbours by enumerating all incident edges and
	// storing those vertices that are not equal to the current vertex
	for(size_t i = 0; i < this->valency(); i++)
	{
		const edge* e = this->get_edge(i);
		if(e->get_u() == this)
			res.push_back(e->get_v());
		else
			res.push_back(e->get_u());
	}

	return(res);
}

/*!
*	Given two vertices (where the first vertex is the current vertex for
*	which this function is called), which are supposed to be the endpoints
*	of an edge, finds the two angles opposite to the edge that is
*	connecting. This function is implemented for triangular meshes only.
*
*	@param v	Second vertex (first vertex is defined implicitly by
*			calling this function)
*
*	@return	The two opposite angles (in radians); errors are indicated by
*		negative angles in both components.
*/

std::pair<double, double> vertex::find_opposite_angles(const vertex* v) const
{
	std::pair<double, double> res(-1.0, -1.0);

	// Shortcut for working with the current vertex within in this function
	const vertex* u = this;

	// Find common edge and obtain the two relevant faces that need to be
	// checked

	const vertex* tmp;	// use vertex with smaller valency in order to speed up
				// the search
	if(u->valency() < v->valency())
		tmp = u;
	else
		tmp = v;

	// store first and second adjacent face of the edge; if one of these is
	// NULL, we will not continue
	const face* faces[2];

	// set to the common edge of vertices u and v by the for-loop below
	const edge* common_edge = NULL;

	for(size_t i = 0; i < tmp->valency(); i++)
	{
		const edge* e = tmp->get_edge(i);
		if(	(e->get_u() == u && e->get_v() == v) ||
			(e->get_u() == v && e->get_v() == u))
		{
			faces[0] = e->get_f();
			faces[1] = e->get_g();

			common_edge = e;
			break;
		}
	}

	if(!common_edge)
	{
		std::cerr	<< "psalm: mesh::find_opposite_angles(): Unable to find common edge. Aborting..."
				<< std::endl;

		return(res);
	}

	if(	!faces[0] ||
		!faces[1])
	{
		// This may happen for boundary edges/vertices and does _not_
		// necessarily indicate an error.
		return(res);
	}

	// Handle misuse of the function
	else if(faces[0]->num_vertices() != 3 || faces[1]->num_vertices() != 3)
	{
		std::cerr	<< "psalm: mesh::find_opposite_angles(): Non-triangular mesh detected. Aborting..."
				<< std::endl;

		return(res);
	}

	// Find remaining two edges for each of the two faces identified above
	// and calculate their angle

	for(size_t i = 0; i < 2; i++)
	{
		edge* e1 = NULL;
		edge* e2 = NULL;

		for(size_t j = 0; j < faces[i]->num_edges(); j++)
		{
			edge* e = faces[i]->get_edge(j).e;
			if(e != common_edge)
			{
				// if the first edge is set, we have found both
				// edges...
				if(e1)
				{
					e2 = e;
					break;
				}
				else
					e1 = e;
			}
		}

		// For the angle calculation, we need to check that both edges
		// point into the _same_ direction -- otherwise the wrong angle
		// would be calculated.

		double angle = 0.0;

		if(	e1->get_u() == e2->get_u() ||
			e2->get_v() == e2->get_v())
		{
			v3ctor a = e1->get_u()->get_position() - e1->get_v()->get_position();
			v3ctor b = e2->get_u()->get_position() - e2->get_v()->get_position();

			angle = acos(a.normalize()*b.normalize());
		}
		else
		{
			v3ctor a = e1->get_u()->get_position() - e1->get_v()->get_position();
			v3ctor b = e2->get_v()->get_position() - e2->get_u()->get_position(); // swap second edge

			angle = acos(a.normalize()*b.normalize());
		}

		// Check where to store the angle
		if(res.first < 0.0)
			res.first = angle;
		else
			res.second = angle;
	}

	return(res);
}

/*!
*	Calculates the Voronoi area of the current vertex. This requires
*	enumerating the 1-ring neighbourhood of the vertex and calculating the
*	opposite angles for incident edges.
*
*	@return Voronoi area of the vertex
*/

double vertex::calc_voronoi_area() const
{
	double area = 0.0;

	std::vector<const vertex*> neighbours = this->get_neighbours();
	if(neighbours.size() == 0)
		return(-1.0);

	for(size_t i = 0; i < neighbours.size(); i++)
	{
		std::pair<double, double> angles = this->find_opposite_angles(neighbours[i]);
		if(angles.first < 0.0 || angles.second < 0.0)
			return(-1.0);

		double distance = (this->get_position() - neighbours[i]->get_position()).length();
		area +=	0.125*(	 1.0/tan(angles.first)
				+1.0/tan(angles.second))
			*distance
			*distance; // using the squared distance is _not_ a typo
	}

	return(area);
}

/*!
*	Calculates the mean curvature around the vertex. This requires
*	enumerating the 1-ring neighbourhood of the vertex.
*
*	@return Mean curvature around the vertex
*/

double vertex::calc_mean_curvature() const
{
	double voronoi_area = this->calc_voronoi_area(); // required for the formula below
	if(std::abs(voronoi_area) < 8*std::numeric_limits<double>::epsilon())
		return(0.0);

	std::vector<const vertex*> neighbours = this->get_neighbours();
	if(neighbours.size() == 0)
		return(0.0);

	// The length of this (non-unit!) normal will be the mean curvature
	v3ctor scaled_normal;

	for(size_t i = 0; i < neighbours.size(); i++)
	{
		std::pair<double, double> angles = this->find_opposite_angles(neighbours[i]);
		if(angles.first < 0.0 || angles.second < 0.0)
			return(0.0);

		scaled_normal +=	 (this->get_position() - neighbours[i]->get_position())
					*(1.0/tan(angles.first) + 1.0/tan(angles.second));
	}

	scaled_normal /= 4.0*voronoi_area;
	return(scaled_normal.length());
}

} // end of namespace "psalm"
