/*!
*	@file	t_edge_hash.h
*	@brief	Class implementing an edge lookup table using a hash map.
*/

#ifndef __T_EDGE_HASH_H__
#define __T_EDGE_HASH_H__

#include <ext/hash_map>

#include "edge.h"
#include "directed_edge.h"

namespace std
{
	using namespace __gnu_cxx;
}

/*!
*	@class	t_edge_hash
*	@brief	A table that stores edges and provides lookup functions. A hash
*		table is used instead of a map.
*/

class t_edge_hash
{
	public:
		directed_edge add(edge& e);
		edge* get(size_t e);

		size_t size();

	private:
		std::vector<edge*>		E;
		std::hash_map<size_t, edge*>	T;
};

#endif
