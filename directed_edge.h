/*!
*	@file	directed_edge.h
*	@brief	Contains directed edge class.
*/

#ifndef __DIRECTED_EDGE_H__
#define __DIRECTED_EDGE_H__

class edge; // forward declaration to break up circular dependency with face class

/*!
*	@class directed_edge
*	@brief	Contains an edge and the edge's direction. Used for performing
*		a lookup in the edge table.
*/

class directed_edge
{
	public:
		const edge* e;
		bool inverted;
};

#endif
