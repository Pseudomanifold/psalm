/*!
*	@file	CatmullClark.h
*	@brief	Class describing Catmull and Clark's subdivision scheme
*/

#ifndef __CATMULL_CLARK_H__
#define __CATMULL_CLARK_H__

#include <utility>
#include "BsplineSubdivisionAlgorithm.h"

namespace psalm
{

/*!
*	@class	CatmullClark
*	@brief	Catmull-Clark subdivision algorithm
*/

class CatmullClark : public BsplineSubdivisionAlgorithm
{
	public:
		CatmullClark();

		bool apply_to(mesh& input_mesh);
		bool set_weights(weights new_weights);

	private:
		void create_face_points(mesh& input_mesh, mesh& output_mesh);
		void create_edge_points(mesh& input_mesh, mesh& output_mesh);
		void create_vertex_points_parametrically(mesh& input_mesh, mesh& output_mesh);
		void create_vertex_points_geometrically(mesh& input_mesh, mesh& output_mesh);

		/*!
			This pointer will be set to an appropriate predefined
			weight function for the Catmull-Clark scheme.

			@see weights_doo_sabin()
			@see weights_catmull_clark()
			@see weights_degenerate()
		*/

		std::pair<double, double> (*weight_function)(size_t);

		static std::pair<double, double> weights_catmull_clark(size_t n);
		static std::pair<double, double> weights_doo_sabin(size_t n);
		static std::pair<double, double> weights_degenerate(size_t n);

		bool non_quadrangular_face;	///< Internal flag signalling that the mesh contains
						///< non-quadrangular faces. In this case, new vertex
						///< points of the Catmull-Clark scheme cannot be
						///< created parametrically.

		bool use_bspline_weights;	///< Flag signalling that B-spline weights shall be
						///< used for all regular parts of a mesh.
};

/*!
*	@param	n Valency of vertex
*	@return	Pair of weights following Doo and Sabin's formula
*/

inline std::pair<double, double> CatmullClark::weights_doo_sabin(size_t n)
{
	return(std::make_pair(1.0/n, 1.0/(4.0*n)));
}

/*!
*	@param	n Valency of vertex
*	@return	Pair of weights following Catmull and Clark's formula
*/

inline std::pair<double, double> CatmullClark::weights_catmull_clark(size_t n)
{
	return(std::make_pair(3.0/(2.0*n), 1.0/(4.0*n)));
}

/*!
*	@param	n Valency of vertex
*	@return	Pair of weights resulting in degenerate meshes
*/

inline std::pair<double, double> CatmullClark::weights_degenerate(size_t n)
{
	return(std::make_pair(0.0, 0.0));
}

} // end of namespace "psalm"

#endif
