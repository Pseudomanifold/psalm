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

		directed_edge* find_edge(const size_t& e, size_t& pos);

		size_t num_edges() const;
		size_t num_vertices() const;

		const vertex* get_vertex(size_t i) const;
		directed_edge get_edge(size_t i);

	private:
		std::vector<directed_edge> E;
		std::vector<const vertex*> V;
};

#endif
