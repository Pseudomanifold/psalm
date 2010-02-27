/*!
*	@file	face.h
*	@brief	Contains face class
*/

#ifndef __FACE_H__
#define __FACE_H__

#include "vertex.h"
#include "directed_edge.h"

/*!
*	@class face
*	@brief Data for a face of the mesh
*/

class face
{
	public:
		void add_edge(const directed_edge& edge);
		void add_vertex(vertex* v);
		void add_face_vertex(vertex* v);

		directed_edge* find_edge(const size_t& e, size_t& pos);

		size_t num_edges() const;
		size_t num_vertices() const;

		void set_id(size_t id);
		size_t get_id() const;

		const vertex* get_vertex(size_t i) const;
		const vertex* get_face_vertex(size_t i) const;
		directed_edge& get_edge(size_t i); // FIXME: Need to check whether this can be made constant

		vertex* face_point; // FIXME: Make this private?

	private:
		std::vector<directed_edge> E;
		std::vector<const vertex*> V;

		std::vector<vertex*> V_F;

		size_t id;
};

#endif
