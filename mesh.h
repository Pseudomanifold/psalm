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

		void load_raw_data(int num_vertices, long* vertex_IDs, double* coordinates);
		void save_raw_data(int* num_new_vertices, double** new_coordinates, int* num_faces, long** vertex_IDs);

		void prune(	const std::set<size_t>& remove_faces,
				const std::set<size_t>& remove_vertices);
		void destroy();
		void replace_with(mesh& M);

		// Functions for modifiying the topology of the mesh

		vertex* add_vertex(double x, double y, double z, size_t id = std::numeric_limits<size_t>::max());
		vertex* add_vertex(const v3ctor& pos, size_t id = std::numeric_limits<size_t>::max());
		void remove_vertex(vertex* v);

		size_t num_vertices() const;
		vertex* get_vertex(size_t i);

		size_t num_edges() const;
		edge* get_edge(size_t i);

		bool relax_edge(edge* e);

		face* add_face(std::vector<vertex*> vertices);
		face* add_face(vertex* v1, vertex* v2, vertex* v3);
		face* add_face(vertex* v1, vertex* v2, vertex* v3, vertex* v4);
		void remove_face(face* f);

		size_t num_faces() const;
		face* get_face(size_t i);

	protected:

		// Data variables

		std::vector<vertex*>	V;
		std::vector<edge*>	E;
		std::vector<face*>	F;

		std::map<std::pair<size_t,size_t>, edge*> E_M;

		size_t id_offset;

		// Functions

		directed_edge add_edge(vertex* u, vertex* v);
		void remove_edge(edge* e);

		std::pair<size_t, size_t> calc_edge_id(const vertex* u, const vertex* v);

		const vertex* find_remaining_vertex(const edge* e, const face* f);
		std::pair<vertex*, vertex*> find_remaining_vertices(const vertex* v, const face* f);
		vertex* find_face_vertex(face* f, const vertex* v);

		std::vector<const vertex*> sort_vertices(face* f, const vertex* v);
		std::vector<face*> sort_faces(vertex* v);

		void mark_boundaries();

		bool load_ply(std::istream& in);
		bool load_obj(std::istream& in);
		bool load_off(std::istream& in);
		bool load_pline(std::istream& in);

		bool save_ply(std::ostream& out);
		bool save_obj(std::ostream& out);
		bool save_off(std::ostream& out);
		bool save_hole(std::ostream& out);
};

/*!
*	Adds a vertex to the mesh. If not specified by the user, the vertex ID
*	is assigned automatically.
*
*	@param x	x position of vertex
*	@param y	y position of vertex
*	@param z	z position of vertex
*	@param id	Vertex ID (by default, this is set to the largest
*			number fitting into a size_t and thus will be ignored)
*
*	@warning The vertices are not checked for duplicates because this
*	function is assumed to be called from internal methods only.
*
*	@return Pointer to new vertex. The pointer remains valid during the
*	lifecycle of the mesh.
*/

inline vertex* mesh::add_vertex(double x, double y, double z, size_t id)
{
	vertex* v;
	if(id != std::numeric_limits<size_t>::max())
		v = new vertex(x,y,z, id);
	else
		v = new vertex(x,y,z, V.size()+id_offset);

	V.push_back(v);
	return(v);
}

/*!
*	Adds a vertex to the mesh. If not specified by the user, the vertex ID
*	is assigned automatically.
*
*	@param pos	Position of the new vertex
*	@param id	Vertex ID (by default, this is set to the largest
*			number fitting into a size_t and thus will be ignored)
*
*	@warning The vertices are not checked for duplicates because this
*	function is assumed to be called from internal methods only.
*
*	@return Pointer to new vertex. The pointer remains valid during the
*	lifecycle of the mesh.
*/

inline vertex* mesh::add_vertex(const v3ctor& pos, size_t id)
{
	return(add_vertex(pos[0], pos[1], pos[2], id));
}

/*!
*	Removes a vertex from the list of vertices and frees allocated memory.
*	This function invalidates the integrity of the list of edges, so it
*	should _only_ be used if all edges have already been processed.
*
*	@param v Vertex to remove from the mesh
*/

inline void mesh::remove_vertex(vertex* v)
{
	std::remove(V.begin(), V.end(), v);
	delete v;
}

/*!
*	@return Number of vertices currently stored in the mesh.
*/

inline size_t mesh::num_vertices() const
{
	return(V.size());
}

/*!
*	@param i Index of desired vertex
*
*	@return ith vertex of the mesh. Caller has to ensure that vertex index
*	is correct.
*/

inline vertex* mesh::get_vertex(size_t i)
{
	return(V[i]);
}

/*!
*	@return Number of edges currently stored in the mesh.
*/

inline size_t mesh::num_edges() const
{
	return(E.size());
}

/*!
*	@param i Index of desired edge
*
*	@return ith edge in the mesh. Caller has to ensure that the edge index
*	is valid.
*/

inline edge* mesh::get_edge(size_t i)
{
	return(E[i]);
}

/*!
*	@return Number of faces currently stored in the mesh.
*/

inline size_t mesh::num_faces() const
{
	return(F.size());
}

/*!
*	@param i Index of desired face
*
*	@return ith face in the mesh. Caller has to ensure that the face index
*	is valid.
*/

inline face* mesh::get_face(size_t i)
{
	return(F[i]);
}

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
