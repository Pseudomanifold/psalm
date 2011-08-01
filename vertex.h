/*!
*	@file	vertex.h
*	@brief	Vertex class
*/

#ifndef __VERTEX_H__
#define __VERTEX_H__

#include <algorithm>
#include <vector>
#include <stdexcept>

#include "v3ctor.h"

namespace psalm
{

class edge; // forward declaration to break up circular dependency
class face; // forward declaration to break up circular dependency

/*!
*	@class vertex
*	@brief Data for a vertex of the mesh
*/

class vertex
{
	public:
		vertex();
		vertex(double x, double y, double z, size_t id);
		vertex(double x, double y, double z, double nx, double ny, double nz, size_t id);

		void set(double x, double y, double z, size_t id);
		void set(double x, double y, double z, double nx, double ny, double nz, size_t id);

		void set_position(const v3ctor& v);
		void set_position(double x, double y, double z);

		const v3ctor& get_position() const;
		const v3ctor& get_normal() const;

		void set_normal(const v3ctor& n);
		void set_normal(double nx, double ny, double nz);

		/*!
		*	@brief Pointer to vertex point.
		*
		*	Pointer to vertex point that corresponds to the current
		*	edge. This pointer is only set and read during
		*	subdivision algorithms and must \e not be relied on
		*	within other functions.
		*/

		vertex* vertex_point;

		void add_edge(edge* e);
		void remove_edge(const edge* e);

		edge* get_edge(size_t i);
		const edge* get_edge(size_t i) const;

		void add_face(const face* f);
		void remove_face(const face* f);
		const face* get_face(size_t i) const;

		size_t get_id() const;
		size_t valency() const;
		size_t num_adjacent_faces() const;

		bool is_on_boundary() const;
		void set_on_boundary(bool boundary = true);

		v3ctor discrete_laplacian() const;
		v3ctor discrete_bilaplacian() const;

		void set_scale_attribute(double value);
		double get_scale_attribute() const;

		std::vector<const vertex*> get_neighbours() const;
		std::vector< std::pair<const face*, const vertex*> > get_1_ring() const;

		std::pair<double, double> find_opposite_angles(const vertex* v) const;
		double find_opposite_angle(const vertex* v, const face* f) const;
		double find_interior_angle(const face* f) const;

		double calc_mean_curvature() const;
		double calc_gaussian_curvature() const;
		double calc_rms_curvature() const;

		double calc_voronoi_area() const;
		double calc_mixed_area() const;
		double calc_ring_area() const;

		size_t region; // XXX: Identifies region the vertex belongs to

	private:
		std::vector<edge*> E;
		std::vector<const face*> F;

		v3ctor p;		///< Position
		v3ctor n;		///< Normal vector (need not be set)

		size_t id;		///< ID (used by the mesh to distinguish between certain types of vertices)
		bool boundary;		///< Flag signalling that the vertex is a boundary vertex

		double scale_attribute;	///< Scale attribute for Liepa's refinement scheme

		double calc_voronoi_region(const vertex* v, const face* f = NULL) const;
};

/*!
*	Sets a new position for the vertex. All other attributes of the vertex
*	remain unchanged.
*
*	@param p New vertex position
*/

inline void vertex::set_position(const v3ctor& p)
{
	this->p[0] = p[0];
	this->p[1] = p[1];
	this->p[2] = p[2];
}

inline void vertex::set_position(double x, double y, double z)
{
	p[0] = x;
	p[1] = y;
	p[2] = z;
}

/*!
*	@return Const reference to vertex position.
*/

inline const v3ctor& vertex::get_position() const
{
	return(p);
}

/*!
*	Sets a new normal for the vertex. All other attributes of the vertex
*	remain unchanged.
*
*	@param n New vertex normal
*/

inline void vertex::set_normal(const v3ctor& n)
{
	this->n[0] = n[0];
	this->n[1] = n[1];
	this->n[2] = n[2];
}

inline void vertex::set_normal(double nx, double ny, double nz)
{
	n[0] = nx;
	n[1] = ny;
	n[2] = nz;
}

/*!
*	@return Const reference to vertex normal.
*/

inline const v3ctor& vertex::get_normal() const
{
	return(n);
}

/*!
*	Sets scale attribute for vertex. The attribute needs to be calculated
*	somewhere else.
*
*	@param value Attribute value
*/

inline void vertex::set_scale_attribute(double value)
{
	this->scale_attribute = value;
}

/*!
*	@return Current value of scale attribute
*/

inline double vertex::get_scale_attribute() const
{
	return(scale_attribute);
}

/*!
*	Removes an edge from the edge references of this vertex. This function
*	is required for operations that change the structure of the mesh.
*	Otherwise, stale edge references would be used in calculations.
*
*	@param	e Edge that is going to be removed
*	@throws	std::runtime_error if the edge could not be found in the edge
*		references of the vertex. This is meant to change errors in
*		performing edge removals.
*/

inline void vertex::remove_edge(const edge* e)
{
	std::vector<edge*>::iterator edge_pos = std::find(E.begin(), E.end(), e);
	if(edge_pos == E.end())
		throw(std::runtime_error("vertex::remove_edge(): Unable to find edge in edge vector"));
	else
		E.erase(edge_pos);
}

} // end of namespace "psalm"

#endif
