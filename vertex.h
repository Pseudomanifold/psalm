/*!
*	@file	vertex.h
*	@brief	Vertex class
*/

#ifndef __VERTEX_H__
#define __VERTEX_H__

#include <vector>
#include "v3ctor.h"

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

		void add_face(const face* f);
		const face* get_face(size_t i) const;

		size_t get_id() const;
		size_t valency() const;
		size_t num_adjacent_faces() const;

	private:
		std::vector<edge*> E;
		std::vector<const face*> F;

		v3ctor p;
		size_t id;
};

#endif
