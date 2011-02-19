/*!
*	@file	PlanarSegmentation.cpp
*	@brief	Functions for the planar segmentation algorithm
*/

#include <list>
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

	this->label_planar_vertices(input_mesh);
	this->label_nonplanar_faces(input_mesh);
	this->label_regions(input_mesh);

	size_t cur_region = 0;
	size_t written = 0;
	do
	{
		for(size_t i = 0; i < input_mesh.num_vertices(); i++)
		{
			if(input_mesh.get_vertex(i)->region != std::numeric_limits<size_t>::max())
			{
				if(input_mesh.get_vertex(i)->region == cur_region)
				{
					written++;
					std::cout << input_mesh.get_vertex(i)->get_position();
				}
			}
		}

		std::cout << "\n\n";

		if(written == 0)
			break;
		else
		{
			cur_region++;
			written = 0;
			continue;
		}
	}
	while(true);

	return(res);

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

/*!
*	Labels all planar vertices of a given input mesh. The result of this
*	function is stored locally within the class.
*
*	@param input_mesh Mesh that is going to be labelled
*/

void PlanarSegmentation::label_planar_vertices(mesh& input_mesh)
{
	planar_vertices.clear();
	for(size_t i = 0; i < input_mesh.num_vertices(); i++)
	{
		vertex* v = input_mesh.get_vertex(i);
		double curvature = v->calc_rms_curvature();

		// TODO: This should use a user-definable threshold value for
		// determining the necessary curvature...
		if(curvature < 0.05)
			planar_vertices.push_back(v);
		else
			nonplanar_vertices.push_back(v);
	}
}

/*!
*	Labels nonplanar faces of the given input mesh. A face is nonplanar if
*	at least one of its vertices is nonplanar.
*
*	@param input_mesh Mesh that is going to be labelled
*/

void PlanarSegmentation::label_nonplanar_faces(mesh& input_mesh)
{
	nonplanar_faces.clear();
	for(size_t i = 0; i < input_mesh.num_faces(); i++)
	{
		face* f = input_mesh.get_face(i);
		for(size_t j = 0; j < f->num_vertices(); j++)
		{
			vertex* v = f->get_vertex(j);

			// TODO: Lookup could be optimized by using maps or
			// fixed-size arrays...
			if(std::find(planar_vertices.begin(), planar_vertices.end(), v) == planar_vertices.end())
			{
				nonplanar_faces.push_back(f);
				break;
			}
		}
	}
}

/*!
*	Labels different regions in the given input mesh. Essentially, this is
*	a breadth-first-search that tries to reach vertices of nonplanar faces,
*	starting with a given vertex of a nonplanar face. Only edges to other
*	nonplanar faces are taken into account. Each face reached from the
*	starting vertex gets assigned the same region.
*
*	@param input_mesh Mesh whose regions are to be labelled
*/

void PlanarSegmentation::label_regions(mesh& input_mesh)
{
	size_t cur_region = 0;
	while(nonplanar_vertices.size() > 0)
	{
		std::list<vertex*> unprocessed_vertices;

		vertex* v = nonplanar_vertices.front();
		if(v->region != std::numeric_limits<size_t>::max())
		{
			nonplanar_vertices.erase(nonplanar_vertices.begin());
			continue;
		}

		unprocessed_vertices.push_back(v);
		while(unprocessed_vertices.size() > 0)
		{
			vertex* v = unprocessed_vertices.front();
			v->region = cur_region;
			std::vector<const vertex*> neighbours = v->get_neighbours();
			for(size_t i = 0; i < neighbours.size(); i++)
			{
				vertex* w = const_cast<vertex*>(neighbours[i]);

				// TODO: Optimize
				if(std::find(planar_vertices.begin(), planar_vertices.end(), w) != planar_vertices.end())
				{
					// skip planar vertices -- only the
					// nonplanar regions should be labelled
					continue;
				}

				if(w->region == std::numeric_limits<size_t>::max())
				{
					w->region = cur_region;
					unprocessed_vertices.push_back(w);
				}
			}

			unprocessed_vertices.pop_front();
		}

		cur_region++;
		nonplanar_vertices.erase(nonplanar_vertices.begin());
	}
	/*
	std::list<vertex*> unprocessed_vertices;
	while(nonplanar_faces.size() > 0)
	{
		face* f = nonplanar_faces.front();
		for(size_t i = 0; i < f->num_vertices(); i++)
		{
			vertex* v = f->get_vertex(i);

			// TODO: Optimize
			if(std::find(planar_vertices.begin(), planar_vertices.end(), v) != planar_vertices.end())
			{
				// skip planar vertices -- only nonplanar
				// regions should be labelled
				continue;
			}

			// Check if vertex has been visited already
			if(v->region == std::numeric_limits<size_t>::max())
			{
				v->region = cur_region;
				unprocessed_vertices.push_back(v);
			}
		}

		nonplanar_faces.erase(nonplanar_faces.begin());

		while(unprocessed_vertices.size() > 0)
		{
			vertex* v = unprocessed_vertices.front();
			std::vector<const vertex*> neighbours = v->get_neighbours();
			for(size_t i = 0; i < neighbours.size(); i++)
			{
				vertex* w = const_cast<vertex*>(neighbours[i]);

				// TODO: Optimize
				if(std::find(planar_vertices.begin(), planar_vertices.end(), w) != planar_vertices.end())
				{
					// skip planar vertices -- only the
					// nonplanar regions should be labelled
					continue;
				}

				if(w->region == std::numeric_limits<size_t>::max())
				{
					w->region = cur_region;
					unprocessed_vertices.push_back(w);
				}
			}

			unprocessed_vertices.pop_front();
		}

		cur_region++;
	}
	*/
}

} // end of namespace "psalm"
