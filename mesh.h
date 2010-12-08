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
#include <limits>

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

		// Enumerating the different file types that may occur
		enum file_type
		{
			TYPE_PLY,
			TYPE_OBJ,
			TYPE_OFF,
			TYPE_EXT
		};

		// Enumerating the different subdivision algorithms that may
		// occur
		enum subdivision_algorithm
		{
			ALG_CATMULL_CLARK,
			ALG_DOO_SABIN,
			ALG_LOOP,
			ALG_LIEPA
		};

		// Enumerating different weight sets for subdivision algorithms
		enum algorithm_weights
		{
			W_DEFAULT,
			W_CATMULL_CLARK,
			W_DOO_SABIN,
			W_DEGENERATE
		};

		// Status flags for use in several routines
		enum status
		{
			STATUS_OK,
			STATUS_ERROR,
			STATUS_UNDEFINED
		};

		mesh();
		~mesh();

		bool load(const std::string& filename, file_type type = TYPE_EXT);
		bool save(const std::string& filename, file_type type = TYPE_EXT);

		void prune(	const std::set<size_t>& remove_faces,
				const std::set<size_t>& remove_vertices);
		void subdivide(	subdivision_algorithm algorithm = mesh::ALG_CATMULL_CLARK,
				size_t steps = 1);
		void destroy();
		void replace_with(mesh& M);

		// Setters for instance options

		void set_crease_handling(bool status= true);
		void set_parametric_point_creation(bool status = true);
		void set_bspline_weights_usage(bool status = true);
		void set_statistics_output(bool status = true);
		void set_boundary_preservation(bool status = true);

		void set_predefined_weights(algorithm_weights weights);
		void set_custom_weights(const weights_map& custom_weights);

	protected:

		// Data variables

		std::vector<vertex*>	V;
		std::vector<edge*>	E;
		std::vector<face*>	F;

		std::map<std::pair<size_t,size_t>, edge*> E_M;

		size_t id_offset;

		// Functions

		vertex* get_vertex(size_t id);

		face* add_face(std::vector<vertex*> vertices);
		face* add_face(vertex* v1, vertex* v2, vertex* v3);
		face* add_face(vertex* v1, vertex* v2, vertex* v3, vertex* v4);
		void remove_face(face* f);

		directed_edge add_edge(const vertex* u, const vertex* v);
		void remove_edge(edge* e);

		std::pair<size_t, size_t> calc_edge_id(const vertex* u, const vertex* v);

		vertex* add_vertex(double x, double y, double z, size_t id = std::numeric_limits<size_t>::max());
		vertex* add_vertex(const v3ctor& pos, size_t id = std::numeric_limits<size_t>::max());

		const vertex* find_remaining_vertex(const edge* e, const face* f);
		std::pair<vertex*, vertex*> find_remaining_vertices(const vertex* v, const face* f);
		vertex* find_face_vertex(face* f, const vertex* v);

		std::vector<const vertex*> sort_vertices(face* f, const vertex* v);
		std::vector<face*> sort_faces(vertex* v);

		void print_progress(std::string op, size_t cur_pos, size_t max_pos);

		void subdivide_loop();
		void subdivide_doo_sabin();
		void subdivide_catmull_clark();
		void subdivide_liepa();

		void ds_create_points_g(mesh& M);
		void ds_create_points_p(mesh& M,
					double (*weight_function)(size_t, size_t));

		void cc_create_points_g(mesh& M);
		void cc_create_points_p(mesh& M,
					std::pair<double, double> (*weight_function)(size_t));


		bool relax_edge(edge* e);

		void mark_boundaries();

		static double ds_weights_ds(size_t k, size_t i);
		static double ds_weights_cc(size_t k, size_t i);
		static double ds_weights_degenerate(size_t k, size_t i);

		static std::pair<double, double> cc_weights_cc(size_t n);
		static std::pair<double, double> cc_weights_ds(size_t n);
		static std::pair<double, double> cc_weights_degenerate(size_t n);

		bool load_ply(std::istream& in);
		bool load_obj(std::istream& in);
		bool load_off(std::istream& in);
		bool load_pline(std::istream& in);

		bool save_ply(std::ostream& out);
		bool save_obj(std::ostream& out);
		bool save_off(std::ostream& out);
		bool save_hole(std::ostream& out);

		bool use_parametric_point_creation;	//< Flag signalling that points in subdivision schemes shall
							//< be computed using the parametric variant. Normally, the
							//< geometric variant is used.

		bool print_statistics;			//< Flag signalling that statistics shall be printed to
							//< STDERR--STDOUT cannot be used because the user may
							//< specify it as an input or output "file".

		bool handle_creases;			//< Flag signalling
							//< that creases or border edges are handled. The handling depends
							//< on the algorithm. The CC scheme, for example, will
							//< compute the midpoint of the crease edges in order to
							//< obtain a new edge point.

		bool preserve_boundaries;		//< Flag signalling
							//< that border vertices are preserved. This means that subdivision
							//< will _preserve_ the original vertices and only create
							//< points in the middle of a boundary edge.

		bool use_bspline_weights;		//< Flag signalling
							//< that the B-spline weights are supposed to be use whenever an
							//< algorithm encounters a regular situation (i.e. 4-sided polygon
							//< for Doo-Sabin, vertex with valency 4 for Catmull-Clark). Only
							//< relevant for parametric point creation.

		short weights;				//< Predefined weight set for the current algorithm; implies
							//< that points are created parametrically

		weights_map ds_custom_weights;		//< Stores custom weights for the DS scheme
};

/*!
*	Adds a triangular face to the mesh. This function allows the caller to
*	specify 3 vertices that will form the new triangle. Thus, specifying a
*	vector of pointers is not necessary.
*
*	@param v1 Pointer to 1st vertex of new face
*	@param v2 Pointer to 2nd vertex of new face
*	@param v3 Pointer to 3rd vertex of new face
*
*	@warning The vertex pointers are not checked for consistency.
*
*	@returns Pointer to new face
*/

inline face* mesh::add_face(vertex* v1, vertex* v2, vertex* v3)
{
	std::vector<vertex*> vertices;

	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);

	return(add_face(vertices));
}

/*!
*	Adds a quadrangular face to the mesh. This function allows the caller
*	to specify 4 vertices that will form the new quadrangle. Thus,
*	specifying a vector of pointers is not necessary.
*
*	@param v1 Pointer to 1st vertex of new face
*	@param v2 Pointer to 2nd vertex of new face
*	@param v3 Pointer to 3rd vertex of new face
*	@param v4 Pointer to 4th vertex of new face
*
*	@warning The vertex pointers are not checked for consistency and
*	planarity.
*
*	@returns Pointer to new face
*/

inline face* mesh::add_face(vertex* v1, vertex* v2, vertex* v3, vertex* v4)
{
	std::vector<vertex*> vertices;

	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v4);

	return(add_face(vertices));
}

/*!
*	@returns ID of the edge described by vertices u and v, which is given
*	as an std::pair sorted by vertex IDs.
*/

inline std::pair<size_t, size_t> mesh::calc_edge_id(const vertex* u, const vertex* v)
{
	std::pair<size_t, size_t> id;

	size_t u_id = u->get_id();
	size_t v_id = v->get_id();

	if(u_id < v_id)
	{
		id.first = u_id;
		id.second = v_id;
	}
	else
	{
		id.first = v_id;
		id.second = u_id;
	}

	return(id);
}

} // end of namespace "psalm"

#endif
