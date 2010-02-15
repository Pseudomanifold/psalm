/*!
*	@file	vertex.h
*	@brief	Vertex class
*/

#ifndef VERTEX_H
#define VERTEX_H

#include <vector>
#include "v3ctor.h"

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
		const v3ctor& get_position();

		size_t v_p; // FIXME: Vertex points should be stored elsewhere.

		void add_incident_edge(const size_t& e);
		size_t& get(const size_t& i);
		size_t	get_id();

		size_t size();

		bool v_v; // FIXME: Flag whether it is a "vertex vertex".

	private:
		std::vector<size_t> E;

		v3ctor p;
		size_t id;
};

#endif
