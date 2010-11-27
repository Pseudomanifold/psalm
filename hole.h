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
		double objective_function(const vertex* v1, const vertex* v2, const vertex* v3) const;

	private:
		size_t** indices;	///< Stores the index that achieves the
					///< minimum for the minimum-weight
					///< triangulation.

		void construct_triangulation(size_t i, size_t k);
};

} // end of namespace "psalm"

#endif
