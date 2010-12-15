/*!
*	@file	DooSabin.h
*	@brief	Class describing Doo and Sabin's subdivision scheme
*/

#ifndef __DOO_SABIN_H__
#define __DOO_SABIN_H__

#include <vector>

#include "SubdivisionAlgorithm.h"

namespace psalm
{

/*!
*	@class DooSabin
*	@brief Doo-Sabin subdivision algorithm
*/

class DooSabin : public SubdivisionAlgorithm
{
	public:
		bool apply_to(mesh& input_mesh);

	private:
		std::vector<face*> sort_faces(vertex* v);
		vertex* find_face_vertex(face* f, const vertex* v);
};

} // end of namespace "psalm"

#endif
