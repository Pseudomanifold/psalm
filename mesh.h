/*!
*	@file	mesh.h
*	@brief	Data structures for representing a mesh
*/

#ifndef __MESH_H__
#define __MESH_H__

#include <iostream>
#include <vector>
#include <string>
#include <tr1/unordered_map>

#include "vertex.h"
#include "directed_edge.h"
#include "edge.h"
#include "face.h"

/*!
*	@class mesh
*	@brief Represents a mesh
*/

class mesh
{
	public:
		mesh();
		~mesh();

		bool load(const std::string& filename, const short type = TYPE_EXT);
		bool save(const std::string& filename, const short type = TYPE_EXT);

		void subdivide_loop();
		void subdivide_doo_sabin();
		void subdivide_catmull_clark();

		void draw();
		void destroy();

		mesh& operator=(const mesh& M);
		void replace_with(mesh& M);

	private:
		std::vector<vertex*>	V;
		std::vector<edge*>	E;
		std::vector<face*>	F;

		std::tr1::unordered_map<size_t, edge*> E_M;

		vertex* get_vertex(size_t id);

		void add_face(const std::vector<size_t>& vertices);
		void add_face(std::vector<vertex*> vertices);

		inline void add_face(vertex* v1, vertex* v2, vertex* v3);
		inline void add_face(vertex* v1, vertex* v2, vertex* v3, vertex* v4);

		directed_edge add_edge(const vertex* u, const vertex* v);

		vertex* add_vertex(double x, double y, double z);
		vertex* add_vertex(const v3ctor& pos);

		const vertex* find_remaining_vertex(const edge* e, const face* f);
		vertex* find_face_vertex(face* f, const vertex* v);

		std::vector<face*> sort_faces(vertex* v);

		bool load_ply(std::istream& in);
		bool load_obj(std::istream& in);
		bool load_off(std::istream& in);

		bool save_ply(std::ostream& out);
		bool save_obj(std::ostream& out);
		bool save_off(std::ostream& out);

		static const short TYPE_PLY;		//< Constant for reading/writing PLY files
		static const short TYPE_OBJ;		//< Constant for reading/writing OBJ files
		static const short TYPE_OFF;		//< Constant for reading/writing OFF files
		static const short TYPE_EXT;		//< Constant for reading/writing files by their extension

		static const short STATUS_OK;		//< Status constant signalling an operation worked
		static const short STATUS_ERROR;	//< Status constant signalling an operation failed
		static const short STATUS_UNDEFINED;	//< Status constant signalling no operation took place
};

#endif
