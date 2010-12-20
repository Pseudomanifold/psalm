/*!
*	@file	DooSabin.cpp
*	@brief	Implementation of Doo and Sabin's subdivision scheme
*/

#include "DooSabin.h"

namespace psalm
{

/*!
*	Sets default attributes for the Doo-Sabin algorithm.
*/

DooSabin::DooSabin()
{
	weight_function = weights_doo_sabin;
}

/*!
*	Sets weights for the Doo-Sabin scheme.
*
*	@param	new_weights New weights for the Doo-Sabin scheme
*	@return	true if the new weights could be set, else false
*/

bool DooSabin::set_weights(weights new_weights)
{
	switch(new_weights)
	{
		case catmull_clark:
			weight_function = weights_catmull_clark;
			break;

		case doo_sabin:
			weight_function = weights_doo_sabin;
			break;

		case degenerate:
			weight_function = weights_degenerate;
			break;

		// weights not found
		default:
			return(false);
	}

	return(true);
}

/*!
*	Applies Doo and Sabin's subdivision algorithm to the given mesh. The
*	mesh will be irreversibly _changed_ by this function.
*
*	@param	input_mesh Mesh on which the algorithm is applied
*	@return	true on success, else false
*/

bool DooSabin::apply_to(mesh& input_mesh)
{
	mesh output_mesh;

	if(use_geometric_point_creation)
		create_face_vertices_geometrically(input_mesh, output_mesh);
	else
		create_face_vertices_parametrically(input_mesh, output_mesh);

	create_f_faces(input_mesh, output_mesh);
	create_e_faces(input_mesh, output_mesh);
	create_v_faces(input_mesh, output_mesh);

	input_mesh.replace_with(output_mesh);
	return(true);
}

/*!
*	Creates the new face vertices for the Doo-Sabin scheme. This algorithm
*	follows the geometrical approach as presented in the paper of Doo and
*	Sabin.
*
*	@param input_mesh	Original input mesh, will not be modified
*	@param output_mesh	Mesh that will contain the new face vertices
*/

void DooSabin::create_face_vertices_geometrically(mesh& input_mesh, mesh& output_mesh)
{
	for(size_t i = 0; i < input_mesh.num_faces(); i++)
	{
		print_progress(	"Creating points [geometrically]",
				i,
				input_mesh.num_faces()-1);

		face* f = input_mesh.get_face(i);

		// Find centroid of face
		v3ctor centroid;
		for(size_t j = 0; j < f->num_vertices(); j++)
		{
			const vertex* v = f->get_vertex(j);
			centroid += v->get_position();
		}
		centroid *= 1.0/f->num_vertices();

		// For a fixed vertex of the face, find the two edges that are
		// incident on this vertex and calculate their midpoints.
		for(size_t j = 0; j < f->num_vertices(); j++)
		{
			const vertex* v = f->get_vertex(j);

			const edge* e1 = NULL;
			const edge* e2 = NULL;
			for(size_t k = 0; k < f->num_edges(); k++)
			{
				if(	f->get_edge(k).e->get_u() == v ||
					f->get_edge(k).e->get_v() == v)
				{
					if(e1 == NULL)
						e1 = f->get_edge(k).e;
					else
					{
						e2 = f->get_edge(k).e;
						break;
					}
				}
			}

			// TODO: Handle error by returning false
			if(e1 == NULL || e2 == NULL)
				throw(std::runtime_error("DooSabin::create_face_vertices_parametrically(): One of the stored edges is NULL -- unable to continue"));

			// Calculate midpoints of the edges and the position of
			// face vertex

			v3ctor midpoint1;
			v3ctor midpoint2;

			midpoint1 = (e1->get_u()->get_position()+e1->get_v()->get_position())/2;
			midpoint2 = (e2->get_u()->get_position()+e2->get_v()->get_position())/2;

			v3ctor face_vertex_position = (midpoint1+midpoint2+centroid+v->get_position())/4;

			// Add new vertex to new mesh and store it in the
			// vector of face vertices for the old face -- this
			// vector will be used when creating the topology of
			// the new mesh.

			vertex* face_vertex = output_mesh.add_vertex(face_vertex_position);
			f->add_face_vertex(face_vertex);
		}
	}
}

/*!
*	Creates the new face vertices for the Doo-Sabin scheme. This algorithm
*	employs a parametrical approach, thereby making it possible for the
*	user to specify different weights in order to fine-tune the algorithm.
*
*	@param input_mesh	Original input mesh, will not be modified
*	@param output_mesh	Mesh that will contain the new face vertices
*/

void DooSabin::create_face_vertices_parametrically(mesh& input_mesh, mesh& output_mesh)
{
	// Only used if extra_weights has been defined
	weights_map::const_iterator it;
	std::vector<double> weights;

	for(size_t i = 0; i < input_mesh.num_faces(); i++)
	{
		print_progress(	"Creating points [parametrically]",
				i,
				input_mesh.num_faces()-1);

		face* f = input_mesh.get_face(i);

		size_t n = f->num_vertices();
		std::vector<const vertex*> vertices = sort_vertices(f, f->get_vertex(0));

		// Check if weights for a face with n vertices can be found
		weights.clear();
		if(	custom_weights.size() != 0 &&
			((it = custom_weights.find(n)) != custom_weights.end()))
			weights = it->second;

		for(size_t j = 0; j < vertices.size(); j++)
		{
			v3ctor face_vertex_position;

			// If user-defined weights are present and weights for the current
			// number of vertices have been found
			if(!weights.empty())
			{
				for(size_t k = 0; k < weights.size(); k++)
					face_vertex_position += vertices[k]->get_position()*weights[k];
			}

			// Use weight distribution function
			else
			{
				// By default, use original weights for quadrangles
				if(n == 4 && use_bspline_weights)
				{
					face_vertex_position =  vertices[0]->get_position()*9.0/16.0+
								vertices[1]->get_position()*3.0/16.0+
								vertices[2]->get_position()*1.0/16.0+
								vertices[3]->get_position()*3.0/16.0;
				}

				else
				{
					for(size_t k = 0; k < vertices.size(); k++)
						face_vertex_position += vertices[k]->get_position()*weight_function(n,k);
				}
			}

			vertex* face_vertex = output_mesh.add_vertex(face_vertex_position);
			f->add_face_vertex(face_vertex);

			// Shift the vector
			const vertex* v = vertices[0];
			vertices.erase(vertices.begin());
			vertices.push_back(v);
		}
	}
}

/*!
*	Creates F-faces for the Doo-Sabin algorithm.
*
*	@param input_mesh	Original input mesh, will not be modified
*	@param output_mesh	Mesh that will contain the new face vertices
*/

void DooSabin::create_f_faces(mesh& input_mesh, mesh& output_mesh)
{
	// Create new F-faces by connecting the appropriate vertex points
	// (generated elsewhere) of the face
	for(size_t i = 0; i < input_mesh.num_faces(); i++)
	{
		print_progress(	"Creating F-faces",
				i,
				input_mesh.num_faces()-1);

		face* f = input_mesh.get_face(i);

		// Since the vertex points are visited in the order of the old
		// vertices, this step is orientation-preserving

		std::vector<vertex*> vertices;
		for(size_t j = 0; j < f->num_vertices(); j++)
			vertices.push_back(f->get_face_vertex(j));

		output_mesh.add_face(vertices);
	}
}

/*!
*	Creates quadrilateral E-faces for the Doo-Sabin algorithm.
*
*	@param input_mesh	Original input mesh, will not be modified
*	@param output_mesh	Mesh that will contain the new face vertices
*/

void DooSabin::create_e_faces(mesh& input_mesh, mesh& output_mesh)
{
	for(size_t i = 0 ; i < input_mesh.num_edges(); i++)
	{
		print_progress(	"Creating E-faces",
				i,
				input_mesh.num_edges()-1);

		edge* e = input_mesh.get_edge(i);

		// Skip border edges--we cannot create any new faces here
		if(e->get_g() == NULL)
			continue;

		/*
			The situation is as follows:

			---------- v ----------
			|          |          |
			|    F     |     G    |
			|          |          |
			|	   |          |
			---------- u ----------

			Since F is the first face that we encountered when
			traversing the edge in its natural direction, we know
			that the orientation is preserved if the corresponding
			face points are connected like:

			u_F -- u_G -- v_G -- v_F -- u_F

		*/

		vertex* v1 = find_face_vertex(e->get_f(), e->get_u());
		vertex* v2 = find_face_vertex(e->get_g(), e->get_u());
		vertex* v3 = find_face_vertex(e->get_g(), e->get_v());
		vertex* v4 = find_face_vertex(e->get_f(), e->get_v());

		output_mesh.add_face(v1, v2, v3, v4);
	}
}

/*!
*	Creates V-faces for the Doo-Sabin algorithm.
*
*	@param input_mesh	Original input mesh, will not be modified
*	@param output_mesh	Mesh that will contain the new face vertices
*/

void DooSabin::create_v_faces(mesh& input_mesh, mesh& output_mesh)
{
	// Create V-faces by connecting the face vertices of all faces that are
	// adjacent to a fixed vertex.
	for(size_t i = 0; i < input_mesh.num_vertices(); i++)
	{
		print_progress("Creating V-faces",
				i,
				input_mesh.num_vertices()-1);

		vertex* v = input_mesh.get_vertex(i);

		// This is a quick fix required for processing some meshes that
		// are degenerate
		if(v->num_adjacent_faces() < 3)
			continue;

		// The faces need to be sorted in counterclockwise order around
		// the vertex.
		std::vector<face*> faces = sort_faces(v);

		// Note that for non-manifold meshes, faces.size() may not be
		// equal to the number of adjacent faces. Faces can only be
		// sorted correctly if a manifold mesh is assumed.
		std::vector<vertex*> vertices;
		for(size_t j = 0; j < faces.size(); j++)
			vertices.push_back(find_face_vertex(faces[j], v));

		output_mesh.add_face(vertices);
	}
}

/*!
*	Given a face and a vertex v, sort all vertices of the face in
*	counterclockwise order, starting with vertex v.
*
*	@param f Pointer to the face
*	@param v Pointer to the "first" vertex
*
*	@return Sorted vector of vertices
*/

std::vector<const vertex*> DooSabin::sort_vertices(face* f, const vertex* v)
{
	std::vector<const vertex*> vertices;

	size_t pos_v = std::numeric_limits<std::size_t>::max();
	directed_edge d_edge_v;

	for(size_t i = 0; i < f->num_edges(); i++)
	{
		d_edge_v = f->get_edge(i);
		if(	d_edge_v.e->get_u() == v ||
			d_edge_v.e->get_v() == v)
		{
			pos_v = i;
			break;
		}
	}

	if(pos_v == std::numeric_limits<size_t>::max())
		throw(std::runtime_error("DooSabin::sort_vertices(): Unable to find vertex"));

	bool take_first = false;	// signals whether the first or the second
					// edge is to be taken for each edge

	if(d_edge_v.e->get_u() == v)
	{
		vertices.push_back(d_edge_v.e->get_u());
		take_first = !d_edge_v.inverted;
	}
	else
	{
		vertices.push_back(d_edge_v.e->get_v());
		take_first = d_edge_v.inverted;
	}

	for(size_t i = pos_v; (i-pos_v) < f->num_edges(); i++)
	{
		const vertex* w;
		directed_edge d_e;

		// Index must wrap around once the end is reached
		if(i >= f->num_edges())
			d_e = f->get_edge(i-f->num_edges());
		else
			d_e = f->get_edge(i);

		// Always take the _same_ vertex of each edge (depending on the
		// position of v)
		if(d_e.inverted)
			w = (take_first ? d_e.e->get_v() : d_e.e->get_u());
		else
			w = (take_first ? d_e.e->get_u() : d_e.e->get_v());

		// Avoid duplicates. They can only occur with v because v will
		// always be the first vertex in the result vector regardless
		// of whether it was the first or second vertex of the edge
		if(w != v)
			vertices.push_back(w);
	}

	return(vertices);
}

/*!
*	Given a vertex, sort all the vertex's adjacent faces in
*	counterclockwise order around the vertex.
*
*	@param v Pointer to vertex
*	@return Sorted vector of faces
*/

std::vector<face*> DooSabin::sort_faces(vertex* v)
{
	std::vector<face*> faces;
	std::vector<edge*> edges;

	for(size_t i = 0; i < v->valency(); i++)
		edges.push_back(v->get_edge(i));

	/*
		The vector of edges is sorted by the following considerations:

			(1)	All incident edges of the vertex are known
			(2)	By using the adjacent faces of an edge, a list of
				adjacent faces can be built:
				(2.1)	Start with any edge
				(2.2)	Search for any other edge which shares
					any face with the start edge. If the edge is
					found, store it after the start edge and repeat
					the process with the new edge.

		This works because there are only two edges which share one
		face. So the next edge that is found will _not_ point to any
		face that has already been processed (i.e., that will _not_ be
		touched by the algorithm anymore).
	*/

	for(size_t i = 0; i < edges.size(); i++)
	{
		for(size_t j = i+1; j < edges.size(); j++)
		{
			if(	edges[j]->get_f() == edges[i]->get_f() ||
				edges[j]->get_g() == edges[i]->get_g() ||
				edges[j]->get_f() == edges[i]->get_g() ||
				edges[j]->get_g() == edges[i]->get_f())
			{
				std::swap(edges[j], edges[i+1]);
				break;
			}
		}
	}

	/*
		From the sorted edges, the faces can be extracted in sorted
		order by checking which faces of the edges coincide: If edges
		e1 and e2 have adjacent faces (f1,f2), (f2,f3), f3 will be
		added to the face vector. The face vector has to be initialized
		with one face, which would be missing otherwise.
	*/

	faces.push_back(edges[0]->get_f());
	for(size_t i = 1; i < edges.size(); i++)
	{
		if(	edges[i]->get_f() == edges[i-1]->get_f() ||
			edges[i]->get_f() == edges[i-1]->get_g())
		{
			// Border edges are simply ignored
			if(edges[i]->get_g() != NULL)
				faces.push_back(edges[i]->get_g());
		}
		else
			faces.push_back(edges[i]->get_f());
	}

	// Check whether orientation is CW or CCW by enumerating all relevant
	// configurations.

	bool revert = false;
	if(edges[0]->get_u() == v)
	{
		if(faces[1] == edges[0]->get_g())
			revert = true;
	}
	else
	{
		if(faces[1] != edges[0]->get_g())
			revert = true;
	}

	if(revert)
		std::reverse(faces.begin(), faces.end());

	return(faces);
}

/*!
*	Given a vertex and a face (of which the vertex is assumed to be a
*	part), find the corresponding face vertex and return a pointer to it.
*
*	@param f Face
*	@param v Vertex, which is assumed to be a part of the face.
*
*	@return Pointer to the face vertex that corresponds to vertex v in the
*	face.
*/

vertex* DooSabin::find_face_vertex(face* f, const vertex* v)
{
	if(f == NULL || v == NULL)
		return(NULL);

	for(size_t i = 0; i < f->num_vertices(); i++)
	{
		// NOTE: Speed could be increased by using lookup tables that
		// map the "old" id to the "new id"
		if(f->get_vertex(i)->get_id() == v->get_id())
			return(f->get_face_vertex(i));
	}

	return(NULL);
}

} // end of namespace "psalm"
