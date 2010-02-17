/*!
*	@file	mesh.h
*	@brief	Data structures for representing a mesh
*/

#ifndef MESH_H
#define MESH_H

#include <vector>
#include <map>

#include "vertex.h"
#include "edge.h"
#include "face.h"

#include "t_face.h"
#include "t_edge_hash.h"

/*!
*	@class mesh
*	@brief Represents a mesh
*/

class mesh
{
	public:
		mesh();
		~mesh();

		bool load_ply(const char* filename);
		bool save_ply(const char* filename);

		void subdivide_loop();
		void subdivide_doo_sabin();
		void subdivide_catmull_clark();

		void draw();
		void destroy();

	private:
		t_edge_hash	edge_table;
		t_face		face_table;

		std::vector<vertex*>	V;
		std::vector<face>	F;

		vertex* get_vertex(size_t id);
		edge* get_edge(size_t e);

		void add_face(std::vector<size_t> vertices);
		void add_vertex(double x, double y, double z);
};

#endif
