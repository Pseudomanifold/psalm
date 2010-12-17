/*!
*	@file	DooSabin.h
*	@brief	Class describing Doo and Sabin's subdivision scheme
*/

#ifndef __DOO_SABIN_H__
#define __DOO_SABIN_H__

#include <vector>
#include <map>

#include "BsplineSubdivisionAlgorithm.h"

namespace psalm
{

/*!
*	Typedef for describing a map that contains the weights for a k-sided
*	face.
*/

typedef std::map<size_t, std::vector<double> > weights_map;

/*!
*	@class DooSabin
*	@brief Doo-Sabin subdivision algorithm
*/

class DooSabin : public BsplineSubdivisionAlgorithm
{
	 public:
		DooSabin();

		bool apply_to(mesh& input_mesh);

	private:
		void create_face_vertices_geometrically(mesh& input_mesh, mesh& output_mesh);
		void create_face_vertices_parametrically(mesh& input_mesh, mesh& output_mesh);

		void create_f_faces(mesh& input_mesh, mesh& output_mesh);
		void create_e_faces(mesh& input_mesh, mesh& output_mesh);
		void create_v_faces(mesh& input_mesh, mesh& output_mesh);

		std::vector<const vertex*> sort_vertices(face* f, const vertex* v);
		std::vector<face*> sort_faces(vertex* v);
		vertex* find_face_vertex(face* f, const vertex* v);

		/*!
			This pointer will be set to an appropriate predefined
			weight function for the Doo-Sabin scheme.

			@see weights_doo_sabin()
			@see weights_catmull_clark()
			@see weights_degenerate()
		*/

		double (*weight_function)(size_t, size_t);

		static double weights_doo_sabin(size_t k, size_t i);
		static double weights_catmull_clark(size_t k, size_t i);
		static double weights_degenerate(size_t k, size_t i);

		/*!
			Custom weights, i.e. weights that are only required for
			certain valencies.
		*/

		weights_map custom_weights;

		bool use_geometric_point_creation;	///< Flag signalling that new face vertices are
							///< supposed to be created geometrically instead
							///< of using parametrical (i.e. weight-based)
							///< methods
};

} // end of namespace "psalm"

#endif
