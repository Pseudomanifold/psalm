/*!
*	@file	edge.h
*	@brief	Edge class
*/

#ifndef __EDGE_H__
#define __EDGE_H__

#include "vertex.h"
#include "face.h"

/*!
*	@class edge
*	@brief Data for an oriented edge (u,v) of the mesh
*/

class edge
{
	public:
		edge();
		edge(const vertex* u, const vertex* v);

		void set(const vertex* u, const vertex* v);
		void set_f(face* f);
		void set_g(face* g);

		const vertex* get_u() const;
		const vertex* get_v() const;

		face* get_f();
		face* get_g();

		const face* get_f() const;
		const face* get_g() const;

		vertex* edge_point; // FIXME: Make this private?

	private:
		const vertex* u;	///< Pointer to start vertex
		const vertex* v;	///< Pointer to end vertex

		face* f;		///< Pointer to first adjacent face
		face* g;		///< Pointer to second adjacent face
};

#endif
