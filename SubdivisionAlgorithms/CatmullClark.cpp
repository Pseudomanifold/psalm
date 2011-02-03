/*!
*	@file	CatmullClark.cpp
*	@brief	Implementation of Catmull and Clark's subdivision scheme
*/

#include "CatmullClark.h"

namespace psalm
{

/*!
*	Sets default attributes for the Catmull-Clark algorithm.
*/

CatmullClark::CatmullClark()
{
	weight_function = weights_catmull_clark;
}

/*!
*	Sets weights for the Catmull-Clark scheme.
*
*	@param	new_weights New weights for the Catmull-Clark scheme
*	@return	true if the new weights could be set, else false
*/

bool CatmullClark::set_weights(weights new_weights)
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
*	Applies Catmull and Clark's subdivision algorithm to the given mesh.
*	The mesh will be irreversibly _changed_ by this function.
*
*	@param	input_mesh Mesh on which the algorithm is applied
*	@return	true on success, else false
*/

bool CatmullClark::apply_to(mesh& input_mesh)
{
	mesh output_mesh;

	create_face_points(input_mesh, output_mesh);
	create_edge_points(input_mesh, output_mesh);

	if(non_quadrangular_face || use_geometric_point_creation)
		create_vertex_points_geometrically(input_mesh, output_mesh);
	else
		create_vertex_points_parametrically(input_mesh, output_mesh);

	/*
		Create new topology of the mesh by connecting

			vertex -- edge -- face -- edge

		points.
	*/

	for(size_t i = 0; i < input_mesh.num_vertices(); i++)
	{
		print_progress("Creating topology",
				i,
				input_mesh.num_vertices()-1);

		vertex* v = input_mesh.get_vertex(i);

		if(v->vertex_point == NULL)
			continue; // ignore degenerate vertices

		for(size_t j = 0; j < v->num_adjacent_faces(); j++)
		{
			const face* f = v->get_face(j);

			// Find the two incident edges of the current vertex
			// that are also part of the current adjacent face

			const edge* e1 = NULL;
			const edge* e2 = NULL;

			for(size_t k = 0; k < v->valency(); k++)
			{
				const edge* e = v->get_edge(k);
				if(e->get_f() == f || e->get_g() == f)
				{
					if(e1 == NULL)
						e1 = e;
					else
					{
						e2 = e;
						break;
					}
				}
			}

			// For non-manifold meshes, we may not be able to find
			// adjacent faces for every combination of vertices and
			// edges
			if(	e1 == NULL ||
				e2 == NULL)
				continue;

			// If crease handling is not enabled, we may not have
			// edge points everywhere. These faces need to be
			// skipped, of course.
			if(	e1->edge_point == NULL ||
				e2->edge_point == NULL)
			{
				if(preserve_boundaries)
				{
					bool u1 = (e1->get_u() == v);
					bool u2 = (e2->get_u() == v);

					vertex* v10;
					vertex* v11;
					vertex* v20;
					vertex* v21;

					if(e1->edge_point == NULL && e2->edge_point == NULL)
					{
						v10 = output_mesh.add_vertex(e1->get_u()->get_position());
						v11 = output_mesh.add_vertex(e1->get_v()->get_position());
						v20 = output_mesh.add_vertex(e2->get_u()->get_position());
						v21 = output_mesh.add_vertex(e2->get_v()->get_position());

						v10->set_on_boundary();
						v11->set_on_boundary();
						v20->set_on_boundary();
						v21->set_on_boundary();

						output_mesh.add_face(v10, f->face_point, v11);
						output_mesh.add_face(v20, f->face_point, v21);
					}
					else if(e1->edge_point != NULL)
					{
						output_mesh.add_face(v->vertex_point, f->face_point, e1->edge_point);
						if(u2)
						{
							v20 = output_mesh.add_vertex(e2->get_v()->get_position());
							v20->set_on_boundary();
							output_mesh.add_face(v->vertex_point, f->face_point, v20);
						}
						else
						{
							v20 = output_mesh.add_vertex(e2->get_u()->get_position());
							v20->set_on_boundary();
							output_mesh.add_face(v->vertex_point, f->face_point, v20);
						}
					}
					else
					{
						output_mesh.add_face(v->vertex_point, f->face_point, e2->edge_point);
						if(u1)
						{
							v10 = output_mesh.add_vertex(e1->get_v()->get_position());
							v10->set_on_boundary();
							output_mesh.add_face(v->vertex_point, f->face_point, v10);
						}
						else
						{
							v10 = output_mesh.add_vertex(e1->get_u()->get_position());
							v10->set_on_boundary();
							output_mesh.add_face(v->vertex_point, f->face_point, v10);
						}
					}

					continue;
				}
				else
					continue; // skip it if boundaries are _not_ to be preserved
			}

			/*
				Check which edge needs to be used first in
				order to orient the new face properly. The
				rationale behind this is to ensure that e1 is
				the _first_ edge that needs to be visited in
				order to get CCW orientation.
			*/

			if(	(e1->get_u()->get_id() == v->get_id() && e1->get_g() == f) ||
				(e1->get_v()->get_id() == v->get_id() && e1->get_f() == f) ||
				(e2->get_u()->get_id() == v->get_id() && e2->get_f() == f) ||
				(e2->get_v()->get_id() == v->get_id() && e2->get_g() == f))
				std::swap(e1, e2);

			output_mesh.add_face(	v->vertex_point,
						e1->edge_point,
						f->face_point,
						e2->edge_point);
		}
	}

	input_mesh.replace_with(output_mesh);
	return(true);
}

/*!
*	Creates face points of Catmull-Clark subdivision and stores them in the
*	new mesh.
*
*	@param input_mesh	Original input mesh, will not be modified
*	@param output_mesh	Mesh that will contain the new vertex points
*/

void CatmullClark::create_face_points(mesh& input_mesh, mesh& output_mesh)
{
	for(size_t i = 0; i < input_mesh.num_faces(); i++)
	{
		print_progress("Creating face points",
				i,
				input_mesh.num_faces()-1);

		face* f = input_mesh.get_face(i);

		v3ctor centroid;
		for(size_t j = 0; j < f->num_vertices(); j++)
			centroid += f->get_vertex(j)->get_position();

		centroid /= f->num_vertices();

		f->face_point = output_mesh.add_vertex(centroid);

		if(!non_quadrangular_face && f->num_vertices() != 4)
			non_quadrangular_face = true;
	}
}

/*!
*	Creates edge points of Catmull-Clark subdivision and stores them in the
*	new mesh.
*
*	@param input_mesh	Original input mesh, will not be modified
*	@param output_mesh	Mesh that will contain the new vertex points
*/

void CatmullClark::create_edge_points(mesh& input_mesh, mesh& output_mesh)
{
	for(size_t i = 0; i < input_mesh.num_edges(); i++)
	{
		print_progress("Creating edge points",
				i,
				input_mesh.num_edges()-1);

		edge* e = input_mesh.get_edge(i);
		v3ctor edge_point;

		// Border/crease edge: Use midpoint of edge for the edge point
		// if crease handling is enabled
		if(e->get_g() == NULL)
		{
			e->edge_point = NULL;
			if(handle_creases && !e->get_u()->is_on_boundary() && !e->get_v()->is_on_boundary())
			{
				edge_point = (	e->get_u()->get_position()+
						e->get_v()->get_position())*0.5;

				e->edge_point = output_mesh.add_vertex(edge_point);
			}

			// Preserve the original boundaries of the object
			else if(preserve_boundaries && e->is_on_boundary())
			// FIXME: else if(preserve_boundaries && e->get_u()->is_on_boundary() && e->get_v()->is_on_boundary())
			{
				/*
				edge_point = (	e->get_u()->get_position()+
						e->get_v()->get_position())*0.5;

				e->edge_point = M.add_vertex(edge_point);
				e->edge_point->set_on_boundary();
				*/

				e->edge_point = NULL;
			}
			else
			{
				// Remove start and end vertex of edge; we will
				// not be able to connect them correctly.

				input_mesh.remove_vertex(e->get_u()); // FIXME: This leads to problems later on...
				input_mesh.remove_vertex(e->get_v());
			}
		}

		// Normal edge
		else
		{
			edge_point = (	e->get_u()->get_position()+
					e->get_v()->get_position()+
					e->get_f()->face_point->get_position()+
					e->get_g()->face_point->get_position())*0.25;

			e->edge_point = output_mesh.add_vertex(edge_point);
		}
	}
}

/*!
*	Creates vertex points of the Catmull-Clark scheme parametrically and
*	stores them in the new mesh.
*
*	@param input_mesh	Original input mesh, will not be modified
*	@param output_mesh	Mesh that will contain the new vertex points
*/

void CatmullClark::create_vertex_points_parametrically(mesh& input_mesh, mesh& output_mesh)
{
	for(size_t i = 0; i < input_mesh.num_vertices(); i++)
	{
		print_progress("Creating vertex points [parametrically]",
				i,
				input_mesh.num_vertices()-1);

		vertex* v = input_mesh.get_vertex(i);

		// Keep boundary vertices if the user chose this behaviour
		if(preserve_boundaries && v->is_on_boundary())
		{
			v->vertex_point = output_mesh.add_vertex(v->get_position());
			v->vertex_point->set_on_boundary();
			continue;
		}

		// will be used later for determining the real weights; for the
		// regular case, we will always use the standard weights
		size_t n = v->valency();
		if(n < 3)
			continue; // ignore degenerate vertices

		double gamma	= 0.0;
		double beta	= 0.0;
		double alpha	= 0.0;

		if(n == 4 && use_bspline_weights)
		{
			gamma	= 1.0/16.0;
			beta	= 3.0/8.0;
			alpha	= 9.0/16.0;
		}
		else
		{
			std::pair<double, double> weights = weight_function(n);

			gamma	= weights.second;
			beta	= weights.first;
			alpha	= 1.0-beta-gamma;
		}

		// sets of vertices with weights beta and gamma
		std::set<const vertex*> vertices_beta;
		std::set<const vertex*> vertices_gamma;

		// All vertices that are connected via an edge with the current
		// vertex will be assigned the weight beta.
		for(size_t j = 0; j < n; j++)
		{
			edge* e = v->get_edge(j);
			if(e->get_u()->get_id() != v->get_id())
				vertices_beta.insert(e->get_u());
			else
				vertices_beta.insert(e->get_v());
		}

		// All remaining vertices of all adjacent faces to the current
		// vertex will be assigned the weight gamma.
		for(size_t j = 0; j < n; j++)
		{
			const face* f = v->get_face(j);
			if(f == NULL)
				continue;

			for(size_t k = 0; k < f->num_vertices(); k++)
			{
				const vertex* f_v = f->get_vertex(k);

				// Insert the vertex only if it is not already
				// counted within in the "beta" set (and if it
				// is not the current vertex)
				if(f_v->get_id() != v->get_id() && vertices_beta.find(f_v) == vertices_beta.end())
					vertices_gamma.insert(f_v);
			}
		}

		// Apply weights; since this is O(n), the function checks
		// whether the weights for beta and gamma are applicable at all

		v3ctor vertex_point = v->get_position()*alpha;

		if(beta != 0.0)
		{
			for(std::set<const vertex*>::iterator it = vertices_beta.begin(); it != vertices_beta.end(); it++)
				vertex_point += (*it)->get_position()*beta/n;
		}

		if(gamma != 0.0)
		{
			for(std::set<const vertex*>::iterator it = vertices_gamma.begin(); it != vertices_gamma.end(); it++)
				vertex_point += (*it)->get_position()*gamma/n;
		}

		v->vertex_point = output_mesh.add_vertex(vertex_point);
	}
}

/*!
*	Creates vertex points of the Catmull-Clark scheme geometrically and
*	stores them in the new mesh.
*
*	@param input_mesh	Original input mesh, will not be modified
*	@param output_mesh	Mesh that will contain the new vertex points
*/

void CatmullClark::create_vertex_points_geometrically(mesh& input_mesh, mesh& output_mesh)
{
	for(size_t i = 0; i < input_mesh.num_vertices(); i++)
	{
		print_progress("Creating vertex points [geometrically]",
				i,
				input_mesh.num_vertices()-1);

		vertex* v = input_mesh.get_vertex(i);

		// Keep boundary vertices if the user chose this behaviour
		if(preserve_boundaries && v->is_on_boundary())
		{
			v->vertex_point = output_mesh.add_vertex(v->get_position());
			v->vertex_point->set_on_boundary();
			continue;
		}

		// This follows the original terminology as described by
		// Catmull and Clark

		v3ctor Q;
		v3ctor R;
		v3ctor S;

		size_t n = v->valency();
		if(n < 3)
			continue; // ignore degenerate vertices

		// Q is the average of the new face points of all faces
		// adjacent to the old vertex point
		for(size_t j = 0; j < v->num_adjacent_faces(); j++)
			Q += v->get_face(j)->face_point->get_position();

		Q /= v->num_adjacent_faces();

		// R is the average of the midpoints of all old edges incident
		// on the current vertex
		for(size_t j = 0; j < n; j++)
		{
			const edge* e = v->get_edge(j);
			R += (e->get_u()->get_position()+e->get_v()->get_position())*0.5;
		}

		R /= n;

		// S is the current vertex
		S = v->get_position();

		v3ctor vertex_point = (Q+R*2+S*(n-3))/n;
		v->vertex_point = output_mesh.add_vertex(vertex_point);
	}
}

} // end of namespace "psalm"
