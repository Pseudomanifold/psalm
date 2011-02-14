/*!
*	@file	edge.h
*	@brief	Edge class
*/

#ifndef __EDGE_H__
#define __EDGE_H__

#include "vertex.h"
#include "face.h"

#include <boost/logic/tribool.hpp>

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
		edge(vertex* u, vertex* v);

		void set(vertex* u, vertex* v);

		vertex*		get_u();
		const vertex*	get_u() const;
		vertex*		get_v();
		const vertex*	get_v() const;

		void set_u(vertex* u);
		void set_v(vertex* v);

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

		bool is_on_boundary();
		void set_on_boundary(bool boundary = true);

		double calc_length() const;
		double calc_angle(const edge* e) const;

	private:

		vertex* u;			///< Pointer to start vertex
		vertex* v;			///< Pointer to end vertex

		face* f;			///< Pointer to first adjacent face
		face* g;			///< Pointer to second adjacent face

		boost::logic::tribool boundary;	///< Flag signalling that the edge is a
						///< boundary edge
};

/*!
*	@returns Length of the edge as the Euclidean distance between its start
*	and end vector.
*/

inline double edge::calc_length() const
{
	v3ctor d = u->get_position() - v->get_position();
	return(d.length()); // XXX: Optimization?
}

} // end of namespace "psalm"

#endif
