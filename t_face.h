/*!
*	@file 	t_face.h
*	@brief	Classes for lookup table for edge --> adjacent faces
*		relationship.
*/

#ifndef __T_FACE_H__
#define __T_FACE_H__

#include <map>

/*!
*	@class 	face_query
*	@brief	Stores adjacent faces for edge
*/

class face_query
{
	public:
		size_t f1;
		size_t f2;
};

/*!
*	@class 	t_face
*	@brief	Lookup table for edge --> adjacent faces
*/

class t_face
{
	public:
		void set_f1(size_t e, size_t f1);
		void set_f2(size_t e, size_t f2);
		
		face_query get(size_t e); 

	private:
		std::map<size_t, face_query> T;
};

#endif
