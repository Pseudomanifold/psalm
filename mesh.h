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
				size_t steps = 1);
		void destroy();

		mesh& operator=(const mesh& M);
		void replace_with(mesh& M);

		// Setters for instance options

		void set_parametric_point_creation(bool status = true);
		void set_predefined_weights(short weights);
		void set_custom_weights(const weights_map& custom_weights);

		// Class-wide constants

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

		// Data variables

		std::vector<vertex*>	V;
		std::vector<edge*>	E;
		std::vector<face*>	F;

		std::tr1::unordered_map<std::string, edge*> E_M;

		// Functions

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

		void subdivide_loop();
		void subdivide_doo_sabin();
		void subdivide_catmull_clark();

		void ds_create_points_g(mesh& M);
		void ds_create_points_p(mesh& M,
					double (*weight_function)(size_t, size_t));

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

		// Status variables and options

		static const short STATUS_OK;		//< Status constant signalling an operation worked
		static const short STATUS_ERROR;	//< Status constant signalling an operation failed
		static const short STATUS_UNDEFINED;	//< Status constant signalling no operation took place

		bool use_parametric_point_creation;	//< Flag signalling that points in subdivision schemes shall
							//< be computed using the parametric variant. Normally, the
							//< geometric variant is used.
		bool print_statistics;			//< Flag signalling that statistics shall be printed to
							//< STDERR--STDOUT cannot be used because the user may
							//< specify it as an input or output "file".

		short weights;				//< Predefined weight set for the current algorithm; implies
							//< that points are created parametrically

		weights_map ds_custom_weights;		//< Stores custom weights for the DS scheme
};

} // end of namespace "psalm"

#endif
