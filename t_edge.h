/*!
*	@file 	t_edge.h
*	@brief	Contains classes related to edge lookup table
*/

#ifndef __T_EDGE_H__
#define __T_EDGE_H__

#include <vector>
#include <map>

#include "edge.h"

/*!
*	@class 	edge_query
*	@brief	Contains edge and direction flag
*/

class edge_query
{
	public:
		size_t e;
		bool inverted;
};

/*!
*	@class 	t_edge
*	@brief 	A table that stores edges and provides
*		lookup functions.
*/

class t_edge
{
	public:
		edge_query add(const edge& e);
		edge& get(size_t e);

		size_t size();

	private:
		std::vector<edge>	E;
		std::map<edge, size_t> 	T;
};

#endif
