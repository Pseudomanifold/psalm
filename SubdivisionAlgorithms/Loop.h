/*!
*	@file	Loop.h
*	@brief	Class describing Loop's subdivision scheme
*/

#ifndef __LOOP_H__
#define __LOOP_H__

#include "SubdivisionAlgorithm.h"

namespace psalm
{

/*!
*	@class Loop
*	@brief Loop subdivision algorithm
*/

class Loop : public SubdivisionAlgorithm
{
	public:
		bool apply_to(mesh& M);

		/*!
		* This subdivision algorithm does not use any weights, hence
		* the function is only a stub.
		*/

		bool set_weights(weights new_weights)
		{
			return(true);
		};

	private:
		void create_vertex_points(mesh& input_mesh, mesh& output_mesh);
		void create_edge_points(mesh& input_mesh, mesh& output_mesh);

		const vertex* find_remaining_vertex(const edge* e, const face* f);
};

} // end of namespace "psalm"

#endif
