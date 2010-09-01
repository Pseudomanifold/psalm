/*!
*	@file	directed_edge.h
*	@brief	Contains class to describe a directed edge.
*/

#ifndef __DIRECTED_EDGE_H__
#define __DIRECTED_EDGE_H__

namespace psalm
{

class edge; // forward declaration to break up circular dependency with face class

/*!
*	@class directed_edge
*	@brief	Contains an edge and the edge's direction.
*
*	Used for performing a lookup in the edge table of the mesh. Directed
*	edges are necessary because in an oriented 2-manifold mesh, each edge
*	appears twice (at least implicitly). For _working_ with edge data,
*	however, edge (a,b) and (b,a) are essentially the same and need _not_
*	be stored twice. Hence, to avoid any duplicates, an edge attribute
*	tells the caller the orientation of the edge.
*/

class directed_edge
{
	public:
		directed_edge();

		edge* e;	///< Pointer to edge
		bool inverted;	///< Flag signalling direction

		/*!
		* @brief Flag signalling a new edge.
		*
		* This flag overrides the "inverted" flag and is used for
		* improperly oriented meshes. The flag is then used by the edge
		* table to signal that the edge is new.
		*/

		bool new_edge;
};

} // end of namespace "psalm"

#endif
