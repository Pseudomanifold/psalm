/*!
*	@file	Loop.cpp
*	@brief	Implementation of Loop's subdivision scheme
*/

#include <cmath>

#include "Loop.h"

namespace psalm
{

/*!
*	Applies Loop's subdivision algorithm to the given mesh. The mesh will
*	be irreversibly _changed_ by this function.
*
*	@param	input_mesh Mesh on which the algorithm is applied
*	@return	true on success, else false
*/

bool Loop::apply_to(mesh& input_mesh)
{
	mesh output_mesh;

	create_vertex_points(input_mesh, output_mesh);
	create_edge_points(input_mesh, output_mesh);

	// Create topology for the new mesh
	for(size_t i = 0; i < input_mesh.num_faces(); i++)
	{
		print_progress("Creating topology", i, input_mesh.num_faces()-1);

		face* f = input_mesh.get_face(i);

		// Check whether the face contains any boundary edges. In this
		// case, normal subdivision rules are not applicable.
		if(	f->get_edge(0).e->is_on_boundary() ||
			f->get_edge(1).e->is_on_boundary() ||
			f->get_edge(2).e->is_on_boundary())
		{
			// Use the vertex points. For boundary vertices, these
			// are already marked as boundary vertices. For
			// interior vertices, these are _slightly_ translated
			// (depending on the structure of the mesh) -- which is
			// a good thing. Otherwise, the subdivision process
			// would be too static.

			vertex* v1 = f->get_vertex(0)->vertex_point;
			vertex* v2 = f->get_vertex(1)->vertex_point;
			vertex* v3 = f->get_vertex(2)->vertex_point;

			v3ctor centroid = (	v1->get_position()+
						v2->get_position()+
						v3->get_position())*(1.0/3.0);

			vertex* v_centre = output_mesh.add_vertex(centroid);

			// Replace triangle by three smaller triangles. The
			// order is correct because the vertices of the face
			// are sorted correctly.

			output_mesh.add_face(v_centre, v1, v2);
			output_mesh.add_face(v_centre, v2, v3);
			output_mesh.add_face(v_centre, v3, v1);

			// Check whether an edge already has an edge point. In
			// this case, a new triangle must be created -- else,
			// the resulting mesh would contain holes.

			for(size_t j = 0; j < 3; j++)
			{
				edge* e = f->get_edge(j).e;
				if(e->edge_point)
				{
					// For each of the edges, we need to
					// check whether the _second_ adjacent
					// face is also a boundary face. In
					// this case, no new face can be
					// created -- otherwise,
					// self-intersections occur.

					bool on_boundary = false;
					if(e->get_f() == f)
						on_boundary = e->get_g()->is_on_boundary();
					else
						on_boundary = e->get_f()->is_on_boundary();

					if(!on_boundary)
					{
						if(j == 0)
							output_mesh.add_face(v2, v1, e->edge_point);
						else if(j == 1)
							output_mesh.add_face(v3, v2, e->edge_point);
						else if(j == 2)
							output_mesh.add_face(v1, v3, e->edge_point);
					}
				}
			}

			continue;
		}

		// ...go through all vertices of the face
		for(size_t j = 0; j < f->num_vertices(); j++)
		{
			/*
				Using the jth vertex of the ith face, we now
				need to find the _two_ adjacent edges for the
				face. This yields one new triangle.
			*/

			size_t n = f->num_edges(); // number of edges in face
			bool assigned_first_edge = false;

			directed_edge d_e1; // first adjacent edge (for vertex & face)
			directed_edge d_e2; // second adjacent edge (for vertex & face)

			// brute-force search for the two edges; could be
			// optimized
			for(size_t k = 0; k < n; k++)
			{
				// TODO: Optimization required.
				directed_edge d_edge = f->get_edge(k);
				if(	d_edge.e->get_u()->get_id() == f->get_vertex(j)->get_id() ||
					d_edge.e->get_v()->get_id() == f->get_vertex(j)->get_id())
				{
					if(!assigned_first_edge)
					{
						d_e1 = d_edge;
						assigned_first_edge = true;
					}
					else
					{
						d_e2 = d_edge;
						break;
					}
				}
			}

			vertex* v1 = f->get_vertex(j)->vertex_point;
			vertex* v2 = d_e1.e->edge_point;
			vertex* v3 = d_e2.e->edge_point;

			/*
				 Create vertices for the _new_ face. It is
				 important to determine the proper order of the
				 edges here. The new edges should run "along"
				 the old ones.

				 Hence, it is checked whether the current
				 vertex is the _start_ vertex of the first
				 edge. This is the case if _either_ the edge is
				 not inverted and the current vertex is equal
				 to the vertex u (start vertex) of the edge
				 _or_ the edge is inverted and the current
				 vertex is equal to the vertex v (end vertex)
				 of the edge.

				 If the current vertex is the start vertex of
				 the first edge, connecting the new points in
				 order v1, v2, v3 will create a face that is
				 oriented counterclockwise. Else, order v1, v3,
				 v2 will have to be used.

				 v1 can remain fixed in all cases because of
				 the symmetry.
			*/

			if((d_e1.e->get_u()->get_id() == f->get_vertex(j)->get_id() && d_e1.inverted == false) ||
			   (d_e1.e->get_v()->get_id() == f->get_vertex(j)->get_id() && d_e1.inverted))
				output_mesh.add_face(v1, v2, v3);

			// Swap order
			else
				output_mesh.add_face(v1, v3, v2);
		}

		// Create face from all three edge points of the face; since
		// the edges are stored in the proper order when adding the
		// face, the order in which the edge points are set will be
		// correct.

		if(f->num_edges() != 3)
		{
			std::cerr << "psalm: Input mesh contains non-triangular face. Loop's subdivision scheme is not applicable.\n";
			return(false);
		}

		output_mesh.add_face(	f->get_edge(0).e->edge_point,
					f->get_edge(1).e->edge_point,
					f->get_edge(2).e->edge_point);
	}

	input_mesh.replace_with(output_mesh);
	return(true);
}

/*!
*	Creates vertex points of Loop subdivision and stores them in the new
*	mesh.
*
*	@param input_mesh	Original input mesh, will not be modified
*	@param output_mesh	Mesh that will contain the new vertex points
*/

void Loop::create_vertex_points(mesh& input_mesh, mesh& output_mesh)
{
	// The vertex points are created by using neighbourhood information of
	// all vertices in the input mesh
	for(size_t i = 0; i < input_mesh.num_vertices(); i++)
	{
		vertex* v = input_mesh.get_vertex(i);
		print_progress("Creating vertex points", i, input_mesh.num_vertices()-1);

		// Preserve boundary vertices if necessary
		if(preserve_boundaries && v->is_on_boundary())
		{
			v->vertex_point = output_mesh.add_vertex(v->get_position());
			v->vertex_point->set_on_boundary();
			continue;
		}

		// Find neighbours

		size_t n = v->valency();
		v3ctor vertex_point;

		// TODO: Iterators required.
		for(size_t j = 0; j < n; j++)
		{
			const edge* e = v->get_edge(j);

			/*
				i is the index of the current vertex; if the
				start vertex of the edge is _not_ the current
				vertex, it must be the neighbouring vertex.
			*/

			const vertex* neighbour = (e->get_u()->get_id() != v->get_id()? e->get_u() : e->get_v());
			vertex_point += neighbour->get_position();
		}

		double s = 0.0;
		if(n > 3)
			s = (1.0/n*(0.625-pow(0.375+0.25*cos(2*M_PI/n), 2)));
		else
			s = 0.1875;

		vertex_point *= s;
		vertex_point += v->get_position()*(1.0-n*s);

		v->vertex_point = output_mesh.add_vertex(vertex_point);
	}
}

/*!
*	Creates edge points of Loop subdivision and stores them in the new
*	mesh.
*
*	@param input_mesh	Original input mesh, will not be modified
*	@param output_mesh	Mesh that will contain the new vertex points
*/

void Loop::create_edge_points(mesh& input_mesh, mesh& output_mesh)
{
	for(size_t i = 0; i < input_mesh.num_edges(); i++)
	{
		print_progress("Creating edge points", i, input_mesh.num_edges()-1);

		v3ctor edge_point;
		edge* e = input_mesh.get_edge(i);

		// Find remaining vertices of the adjacent faces of the edge
		const vertex* v1 = find_remaining_vertex(e, e->get_f());
		const vertex* v2 = find_remaining_vertex(e, e->get_g());

		// Boundary edge: Use midpoint subdivision
		if(v1 == NULL || v2 == NULL)
		{
			edge_point = (	(e->get_u()->get_position()+
					 e->get_v()->get_position())*0.5);
		}

		// Normal edge
		else
		{
			edge_point =	(e->get_u()->get_position()+e->get_v()->get_position())*0.375+
					(v1->get_position()+v2->get_position())*0.125;
		}

		if(v1 != NULL && v2 != NULL)
			e->edge_point = output_mesh.add_vertex(edge_point);
		else
			e->edge_point = NULL;

		// XXX: Check whether this is correct
		if(preserve_boundaries && (v1 == NULL || v2 == NULL))
			e->edge_point->set_on_boundary();
	}
}

/*!
*	Given an edge and a triangular face (where the edge is supposed to be
*	part of the face), return the remaining vertex of the face.
*
*	@param e Edge
*	@param f Face that is adjacent to the edge. The face is supposed to
*	have only 3 vertices.
*
*	@return Pointer to the remaining vertex of the face or NULL if the
*	vertex could not be found.
*/

const vertex* Loop::find_remaining_vertex(const edge* e, const face* f)
{
	if(f == NULL || e == NULL)
		return(NULL);

	const vertex* result = NULL;
	for(size_t i = 0; i < 3; i++)
	{
		// The IDs of the start and end vertices of the edge
		// differ from the ID of the remaining edge.
		if(	f->get_vertex(i)->get_id() != e->get_u()->get_id() &&
			f->get_vertex(i)->get_id() != e->get_v()->get_id())
		{
			result = f->get_vertex(i);
			break;
		}
	}

	return(result);
}

} // end of namespace "psalm"
