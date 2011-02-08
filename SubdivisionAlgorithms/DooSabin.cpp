/*!
*	@file	DooSabin.cpp
*	@brief	Implementation of Doo and Sabin's subdivision scheme
*/

#include "DooSabin.h"

namespace psalm
{

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

	// Create new F-faces by connecting the appropriate vertex points
	// (generated above) of the face
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

	// Create quadrilateral E-faces
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

	return(true);
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
