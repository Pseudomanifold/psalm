/*!
*	@file	vertex.h
*	@brief	Vertex class
*/

#ifndef VERTEX_H
#define VERTEX_H

#include <vector>

#include "v3ctor.h"

class edge; // forward declaration to break up circular dependency

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

		vertex* vertex_point; // FIXME: Make this private?

		void add_edge(const edge* e);
		const edge* get_edge(size_t i);

		size_t get_id() const;
		size_t valency() const;

	private:
		std::vector<const edge*> E;

		v3ctor p;
		size_t id;
};

#endif
