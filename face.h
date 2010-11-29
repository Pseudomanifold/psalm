/*!
*	@file	face.h
*	@brief	Contains face class
*/

#ifndef __FACE_H__
#define __FACE_H__

#include "vertex.h"
#include "directed_edge.h"

namespace psalm
{

/*!
*	@class face
*	@brief Data for a face of the mesh
*/

class face
{
	public:
		face();

		void add_edge(const directed_edge& edge);
		void add_vertex(vertex* v);
		void add_face_vertex(vertex* v);

		size_t num_edges() const;
		size_t num_vertices() const;

		const vertex* get_vertex(size_t i) const;
		vertex* get_face_vertex(size_t i);
		directed_edge& get_edge(size_t i);

		/*!
		*	@brief Pointer to face point.
		*
		*	Pointer to face point that corresponds to the current
		*	face. This pointer is only set and read during
		*	subdivision algorithms and must \e not be relied on
		*	within other functions.
		*/

		vertex* face_point;

		bool is_on_boundary() const;
		void set_on_boundary(bool boundary = true);

		void reconstruct_from_edges();

	private:
		std::vector<directed_edge> E;
		std::vector<const vertex*> V;

		std::vector<vertex*> V_F;

		size_t id;

		bool boundary;	///< Flag signalling that the face is a
				///< boundary face.
};

} // end of namespace "psalm"

#endif
