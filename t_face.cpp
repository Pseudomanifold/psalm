/*!
*	@file	t_face.cpp
*	@brief	Functions for face lookup table
*/

#include "t_face.h"

/*!
*	Assigns first face for specified edge.
*
*	@param e 	Edge
*	@param f1	First face to assign to the edge
*/

void t_face::set_f1(size_t e, size_t f1)
{
	T[e].f1 = f1;
}

/*!
*	Assigns second face for specified edge.
*
*	@param e 	Edge
*	@param f2	First face to assign to the edge
*/

void t_face::set_f2(size_t e, size_t f2)
{
	T[e].f2 = f2;
}

/*!
*	Returns specified edge from edge vector.
*/

face_query& t_face::get(size_t e)
{
	return(T[e]);
}
