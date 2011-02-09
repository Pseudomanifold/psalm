/*!
*	@file	libpsalm.h
*	@brief	Exports a function for filling holes. Used by GigaMesh.
*	@author Bastian Rieck <bastian.rieck@iwr.uni-heidelberg.de>
*/

#ifndef __LIBPSALM_H__
#define __LIBPSALM_H__

bool fill_hole(	int num_vertices, long* vertex_IDs, double* coordinates, double* scale_attributes, double* normals,
		int* num_new_vertices, double** new_coordinates, int* num_new_faces, long** new_vertex_IDs);

#endif
