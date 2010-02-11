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
		v3ctor p;

		void add_incident_edge(const size_t& e);
		size_t& get(const size_t& i);
		
		size_t size();

	private:
		std::vector<size_t> E;
};

#endif
