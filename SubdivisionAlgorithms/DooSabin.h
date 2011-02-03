/*!
*	@file	DooSabin.h
*	@brief	Class describing Doo and Sabin's subdivision scheme
*/

#ifndef __DOO_SABIN_H__
#define __DOO_SABIN_H__

#include <cmath>
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
		bool set_weights(weights new_weights);

		void set_custom_weights(const weights_map& custom_weights);

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
};

/*!
*	Allows the user to set custom weights for the Doo-Sabin subdivision
*	scheme.
*
*	@param custom_weights The new weights
*/

inline void DooSabin::set_custom_weights(const weights_map& custom_weights)
{
	this->custom_weights = custom_weights;
}


/*!
*	Computes the weight factor for the ith vertex of a face with k
*	vertices. The formula of Doo and Sabin is used.
*
*	@param i Index of vertex in face (0, 1, ..., k-1)
*	@param k Number of vertices
*
*	@return Weight
*/

inline double DooSabin::weights_doo_sabin(size_t k, size_t i)
{
	if(i == 0)
		return(0.25+5.0/(4.0*k));
	else
		return((3.0+2.0*cos(2*M_PI*i/k))/(4.0*k));
}

/*!
*	Computes the weight factor for the ith vertex of a face with k
*	vertices. The formula of Catmull and Clark is used.
*
*	@param i Index of vertex in face (0, 1, ..., k-1)
*	@param k Number of vertices
*
*	@return Weight
*/

inline double DooSabin::weights_catmull_clark(size_t k, size_t i)
{
	if(i == 0)
		return(0.5+1.0/(4.0*k));
	else
	{
		if(i == 1 || i == (k-1))
			return(0.125+1.0/(4.0*k));
		else
			return(1.0/(4.0*k));
	}
}

/*!
*	Computes the weight factor for the ith vertex of a face with k
*	vertices. The weights have been selected in order to yield the
*	most degenerate surfaces.
*
*	@param i Index of vertex in face (0, 1, ..., k-1)
*	@param k Number of vertices
*
*	@return Weight
*/

inline double DooSabin::weights_degenerate(size_t k, size_t i)
{
	if(i == 0)
		return(0.0);
	else
		return(1.0/static_cast<double>(k-1));
}

} // end of namespace "psalm"

#endif
