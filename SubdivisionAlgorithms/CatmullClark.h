/*!
*	@file	CatmullClark.h
*	@brief	Class describing Catmull and Clark's subdivision scheme
*/

#ifndef __CATMULL_CLARK_H__
#define __CATMULL_CLARK_H__

#include "SubdivisionAlgorithm.h"

namespace psalm
{

/*!
*	@class	CatmullClark
*	@brief	Catmull-Clark subdivision algorithm
*/

class CatmullClark : public SubdivisionAlgorithm
{
	public:
		bool apply_to(mesh& input_mesh);

	private:
		void create_face_points(mesh& input_mesh, mesh& output_mesh);
		void create_edge_points(mesh& input_mesh, mesh& output_mesh);
		void create_vertex_points_parametrically(mesh& input_mesh, mesh& output_mesh);
		void create_vertex_points_geometrically(mesh& input_mesh, mesh& output_mesh);

		bool non_quadrangular_face;	///< Internal flag signalling that the mesh contains
						///< non-quadrangular faces. In this case, new vertex
						///< points of the Catmull-Clark scheme cannot be
						///< created parametrically.
};

} // end of namespace "psalm"

#endif
