/*!
*	@file	PlanarSegmentation.cpp
*	@brief	Functions for the planar segmentation algorithm
*/

#include "PlanarSegmentation.h"

namespace psalm
{

/*!
*	Performs a planar segmentation to the input mesh: Planar vertices are
*	identified by calculating the discrete mean curvature of a vertex. If
*	all planar vertices have been identified, edges leading to planar
*	vertices will be inserted into a new, which is returned as the
*	function's result.
*
*	@param	input_mesh Mesh for which the planar segmentation shall be
*		performed
*
*	@return	Segmented mesh
*/

mesh PlanarSegmentation::apply_to(mesh& input_mesh)
{
	mesh res;

	/*
	*	FIXME: This assumes that the vertex IDs are numbered
	*	sequentially. If this is not the case, the function will
	*	fail...
	*/

	// Identify planar vertices

	size_t n = input_mesh.num_vertices();
	bool* is_planar = new bool[n];

	for(size_t i = 0; i < n; i++)
	{
		vertex* v = input_mesh.get_vertex(i);
		double curvature = v->calc_mean_curvature();

		if(curvature < 0.005)
			is_planar[i] = true;
		else
			is_planar[i] = false;
	}

	for(size_t i = 0; i < input_mesh.num_faces(); i++)
	{
		face* f = input_mesh.get_face(i);

		size_t non_planar = 0;
		for(size_t j = 0; j < f->num_vertices(); j++)
		{
			if(!is_planar[f->get_vertex(j)->get_id()])
				non_planar++;
		}

		if(non_planar > 1)
			continue;

		vertex* v1 = res.add_vertex(f->get_vertex(0)->get_position());
		vertex* v2 = res.add_vertex(f->get_vertex(1)->get_position());
		vertex* v3 = res.add_vertex(f->get_vertex(2)->get_position());

		res.add_face(v1,v2,v3);
	}

	delete[] is_planar;
	return(res);
}

} // end of namespace "psalm"
