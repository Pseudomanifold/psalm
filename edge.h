/*!
*	@file	edge.h
*	@brief	Edge class
*/

#ifndef __EDGE_H__
#define __EDGE_H__

#include "vertex.h"
#include "face.h"

namespace psalm
{

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

		const vertex* get_u() const;
		const vertex* get_v() const;

		void set_f(face* f);
		void set_g(face* g);

		face*		get_f();
		const face*	get_f() const;
		face*		get_g();
		const face*	get_g() const;

		/*!
		*	@brief Pointer to edge point.
		*
		*	Pointer to edge point that corresponds to the current
		*	edge.  This pointer is only set and read during
		*	subdivision algorithms and must \e not be relied on
		*	within other functions.
		*/

		vertex* edge_point;

	private:
		const vertex* u;	///< Pointer to start vertex
		const vertex* v;	///< Pointer to end vertex

		face* f;		///< Pointer to first adjacent face
		face* g;		///< Pointer to second adjacent face
};

} // end of namespace "psalm"

#endif
