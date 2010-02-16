/*!
*	@file	edge.h
*	@brief	Edge class
*/

#ifndef __EDGE_H__
#define __EDGE_H__

#include <iostream>

class vertex; // forward declaration to break up circular dependency

/*!
*	@class edge
*	@brief Data for an oriented edge (u,v) of the mesh
*/

class edge
{
	public:
		edge();
		edge(vertex* u, vertex* v);

		void set(vertex* u, vertex* v);

		vertex* get_u();
		vertex* get_v();

		size_t f1;
		size_t f2;

		size_t e_p; // FIXME: Edge points should be stored elsewhere.

		bool operator<(const edge& b) const;

	private:
		vertex* u; ///< Pointer to start vertex
		vertex* v; ///< Pointer to end vertex
};

#endif
