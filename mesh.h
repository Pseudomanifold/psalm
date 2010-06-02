/*!
*	@file	mesh.h
*	@brief	Data structures for representing a mesh
*/

#ifndef __MESH_H__
#define __MESH_H__

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <set>
#include <map>
#include <tr1/unordered_map>

#include "vertex.h"
#include "directed_edge.h"
#include "edge.h"
#include "face.h"

namespace psalm
{

/*!
*	Typedef for describing a map that contains the weights for an k-sided
*	face.
*/

typedef std::map<size_t, std::vector<double> > weights_map;

/*!
*	@class mesh
*	@brief Represents a mesh
*/

class mesh
{
	public:
		mesh();
		~mesh();

		bool load(const std::string& filename, short type = TYPE_EXT);
		bool save(const std::string& filename, short type = TYPE_EXT);

		void prune(const std::set<size_t>& ignore_faces);
		void subdivide(	short algorithm = mesh::ALG_CATMULL_CLARK,
				size_t steps = 1,
				short weights = mesh::W_DEFAULT,
				const weights_map* extra_weights = NULL);
		void destroy();

		mesh& operator=(const mesh& M);
		void replace_with(mesh& M);

		static const short TYPE_PLY;		///< Constant for reading/writing PLY files
		static const short TYPE_OBJ;		///< Constant for reading/writing OBJ files
		static const short TYPE_OFF;		///< Constant for reading/writing OFF files
		static const short TYPE_EXT;		///< Constant for reading/writing files by their extension

		static const short ALG_CATMULL_CLARK;	///< Represents Catmull-Clark algorithm in subdivision method
		static const short ALG_DOO_SABIN;	///< Represents Doo-Sabin algorithm in subdivision method
		static const short ALG_LOOP;		///< Represents Loop algorithm in subdivision method

		static const short W_DEFAULT;		///< Represents default weights for any subdivision scheme
		static const short W_CATMULL_CLARK;	///< Represents Catmull-Clark weights for the DS scheme
		static const short W_DOO_SABIN;		///< Represents Doo-Sabin weights for the DS scheme

	private:
		std::vector<vertex*>	V;
		std::vector<edge*>	E;
		std::vector<face*>	F;

		std::tr1::unordered_map<std::string, edge*> E_M;

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

		std::vector<const vertex*> sort_vertices(face* f, const vertex* v);
		std::vector<face*> sort_faces(vertex* v);

		void subdivide_loop(short weights, const weights_map* extra_weights);
		void subdivide_doo_sabin(short weights, const weights_map* extra_weights);
		void subdivide_catmull_clark(short weights, const weights_map* extra_weights);

		void ds_create_points_g(mesh& M);
		void ds_create_points_p(mesh& M,
					double (*weight_function)(size_t, size_t),
					const weights_map*);

		void cc_create_points_g(mesh& M);
		void cc_create_points_p(mesh& M,
					std::pair<double, double> (*weight_function)(size_t));

		static double ds_weights_ds(size_t k, size_t i);
		static double ds_weights_cc(size_t k, size_t i);

		static std::pair<double, double> cc_weights_cc(size_t n);

		bool load_ply(std::istream& in);
		bool load_obj(std::istream& in);
		bool load_off(std::istream& in);

		bool save_ply(std::ostream& out);
		bool save_obj(std::ostream& out);
		bool save_off(std::ostream& out);

		static const short STATUS_OK;		//< Status constant signalling an operation worked
		static const short STATUS_ERROR;	//< Status constant signalling an operation failed
		static const short STATUS_UNDEFINED;	//< Status constant signalling no operation took place
};

} // end of namespace "psalm"

#endif
