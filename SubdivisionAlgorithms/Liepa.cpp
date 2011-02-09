/*!
*	@file	Liepa.cpp
*	@brief	Implementation of Liepa's subdivision scheme
*/

#include <cmath>
#include "Liepa.h"

namespace psalm
{

/*!
*	Sets default values for Liepa subdivision.
*/

Liepa::Liepa()
{
	alpha = sqrt(2);
}

/*!
*	Sets current value of density parameter for the algorithm.
*
*	@param alpha New value for density parameter
*/

void Liepa::set_alpha(double alpha)
{
	this->alpha = alpha;
}

/*!
*	@returns Current value of density parameter for the algorithm.
*/

double Liepa::get_alpha()
{
	return(alpha);
}

/*!
*	Applies Liepa's subdivision scheme to the given mesh. The mesh will be
*	irreversibly _changed_ by this function.
*
*	@param	input_mesh Mesh on which the algorithm is applied
*	@return	true on success, else false
*/

bool Liepa::apply_to(mesh& input_mesh)
{
	/*
		Compute scale attribute as the average length of the edges
		adjacent to a vertex.

		ASSUMPTION: Mesh consists of a single triangulated hole, i.e.
		_all_ vertices are boundary vertices.
	*/

	for(size_t i = 0; i < input_mesh.num_vertices(); i++)
	{
		vertex* v = input_mesh.get_vertex(i);
		size_t n = v->valency();

		double attribute		= 0.0;
		bool found_first_boundary_edge	= false;	// in the triangulated mesh, there are
								// only two boundary edges per vertex;
								// if we have found those two, the search
								// may be stopped.
		for(size_t i = 0; i < n; i++)
		{
			edge* e = v->get_edge(i);
			if(e->is_on_boundary())
			{
				attribute += 0.5*e->calc_length();

				// break if we have already seen a boundary
				// edge -- see discussion above
				if(found_first_boundary_edge)
					break;
				else
					found_first_boundary_edge = true;
			}
		}

		// If the scale attributes have already been seeded, their
		// average is taken to be the new scale attribute...
		double old_attribute = v->get_scale_attribute();
		if(old_attribute != 0.0)
			v->set_scale_attribute(0.5*(old_attribute + attribute));
		else
			v->set_scale_attribute(attribute);
	}

	bool created_new_triangle;
	do
	{
		// if no new triangle has been created, the algorithm
		// terminates
		created_new_triangle = false;

		// Need to store the number of faces here because new faces
		// might be created within the for-loop below. These must _not_
		// be considered in the same iteration.
		size_t num_faces = input_mesh.num_faces();

		// Compute scale attribute for each face of the mesh
		for(size_t i = 0; i < num_faces; i++)
		{
			face* f = input_mesh.get_face(i);
			if(f->num_edges() != 3)
			{
				std::cerr << "psalm: Input mesh contains non-triangular face. Liepa's subdivision scheme is not applicable.\n";
				return(false);
			}

			vertex* vertices[3];
			vertices[0] = f->get_vertex(0);
			vertices[1] = f->get_vertex(1);
			vertices[2] = f->get_vertex(2);

			// Compute centroid and scale attribute. If the scale
			// attribute test fails, replace the triangle.

			v3ctor centroid_pos;
			double centroid_scale_attribute = 0.0;

			for(size_t j = 0; j < 3; j++)
			{
				centroid_pos += vertices[j]->get_position()/3.0;
				centroid_scale_attribute += vertices[j]->get_scale_attribute()/3.0;
			}

			size_t tests_failed = 0;
			for(size_t j = 0; j < 3; j++)
			{
				double scaled_distance = alpha*(centroid_pos - vertices[j]->get_position()).length();
				if(	scaled_distance > centroid_scale_attribute &&
					scaled_distance > vertices[j]->get_scale_attribute())
				{
					// We will replace the triangle only if
					// _all_ three tests failed
					tests_failed++;
				}
			}

			// Replace old triangle with three smaller triangles
			if(tests_failed == 3)
			{
				created_new_triangle = true;

				vertex* centroid_vertex = input_mesh.add_vertex(centroid_pos);
				centroid_vertex->set_scale_attribute(centroid_scale_attribute);

				// Remove old face and replace it by three new
				// faces. Calling remove_face() will ensure
				// that the edges are updated correctly.

				input_mesh.remove_face(f);
				delete f;

				face* new_face1 = input_mesh.add_face(vertices[0], vertices[1], centroid_vertex, true);
				face* new_face2 = input_mesh.add_face(centroid_vertex, vertices[1], vertices[2], true);
				face* new_face3 = input_mesh.add_face(vertices[0], centroid_vertex, vertices[2], true);

				if(!new_face1 || !new_face2 || !new_face3)
				{
					std::cerr << "psalm: Error: Liepa::apply_to(): Unable to add new face\n";
					return(false);
				}

				num_faces--;
				i--;

				// Relax edges afterwards to maintain
				// Delaunay-like mesh

				input_mesh.relax_edge(new_face1->get_edge(0).e);
				input_mesh.relax_edge(new_face2->get_edge(1).e);
				input_mesh.relax_edge(new_face3->get_edge(2).e);
			}
		}

		if(!created_new_triangle)
			return(true);

		// Relax interior edges
		bool relaxed_edge;
		do
		{
			relaxed_edge = false;
			for(size_t i = 0; i < input_mesh.num_edges(); i++)
			{
				if(input_mesh.relax_edge(input_mesh.get_edge(i)))
					relaxed_edge = true;
			}
		}
		while(relaxed_edge);

		/*
			XXX: This might lead to wrong results...

			// Calculate new scaling attributes. TODO: This should become a
			// function.
			for(size_t i = 0; i < input_mesh.num_vertices(); i++)
			{
				vertex* v = input_mesh.get_vertex(i);
				size_t n = v->valency();

				if(!v->is_on_boundary())
					continue;

				double attribute = 0.0;
				for(size_t i = 0; i < n; i++)
					attribute += v->get_edge(i)->calc_length()/static_cast<double>(n);

				v->set_scale_attribute(attribute);
			}
		*/

		if(!relaxed_edge)
			continue;
	}
	while(created_new_triangle);

	return(true);
}

} // end of namespace "psalm"
