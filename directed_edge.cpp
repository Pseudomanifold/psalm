/*!
*	@file	directed_edge.cpp
*	@brief	Functions for describign a directed edge.
*/

#include <cstddef>
#include "directed_edge.h"

namespace psalm
{

/*!
*	Default constructor. Sets all attributes to "false" or "NULL".
*/

directed_edge::directed_edge()
{
	e		= NULL;
	inverted	= false;
	new_edge	= false;
}

} // end of namespace "psalm"
