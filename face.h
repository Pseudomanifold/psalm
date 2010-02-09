/*!
*	@file 	face.h
*	@brief	Contains face class
*/

#ifndef __FACE_H__
#define __FACE_H__

/*!
*	@class face
*	@brief Data for a face of the mesh
*/

#include "t_edge.h"

class face
{
	public:
		void add_edge(const edge_query& result);

	private:
		std::vector<edge_query> E;
		std::vector<size_t> V;
};

#endif
