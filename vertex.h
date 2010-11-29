/*!
*	@file	vertex.h
*	@brief	Vertex class
*/

#ifndef __VERTEX_H__
#define __VERTEX_H__

#include <vector>
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

		void set(double x, double y, double z, size_t id);
		void set_position(const v3ctor& v);
		const v3ctor& get_position() const;

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
		edge* get_edge(size_t i);
		const edge* get_edge(size_t i) const;

		void add_face(const face* f);
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

	private:
		std::vector<edge*> E;
		std::vector<const face*> F;

		v3ctor p;
		size_t id;
		bool boundary;		///< Flag signalling that the vertex is a boundary vertex

		double scale_attribute;	///< Scale attribute for Liepa's refinement scheme
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

/*!
*	@return Const reference to vertex position.
*/

inline const v3ctor& vertex::get_position() const
{
	return(p);
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


} // end of namespace "psalm"

#endif
