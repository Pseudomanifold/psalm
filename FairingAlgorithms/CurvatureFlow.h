/*!
*	@file	CurvatureFlow.h
*	@brief	Describes a fairing algorithm working with curvature flows
*/

#ifndef __CURVATURE_FLOW_H__
#define __CURVATURE_FLOW_H__

#include "FairingAlgorithm.h"

namespace psalm
{

/*!
*	@class CurvatureFlow
*	@brief Fairing/smoothing algorithm based on curvature flows
*/

class CurvatureFlow : public FairingAlgorithm
{
	public:
		CurvatureFlow();

		bool apply_to(mesh& input_mesh);
};

} // end of namespace "psalm"

#endif
