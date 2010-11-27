/*!
*	@file	hole.h
*	@brief	Data structure for describing a polygonal hole
*/

#ifndef __HOLE_H__
#define __HOLE_H__

#include "mesh.h"

namespace psalm
{

/*!
*	@class hole
*	@brief Represents polygonal holes
*/

class hole : public mesh
{
	public:
		void initialize(const std::vector<v3ctor>& vertices);
		void triangulate();

	protected:
		inline double objective_function(vertex* v1, vertex* v2, vertex* v3);

	private:
		size_t** indices;	///< Stores the index that achieves the
					///< minimum for the minimum-weight
					///< triangulation.

		void construct_triangulation(size_t i, size_t k);
};

} // end of namespace "psalm"

#endif
