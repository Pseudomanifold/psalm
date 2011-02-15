/*!
*	@file	CurvatureFlow.h
*	@brief	Describes a fairing algorithm working with curvature flows
*/

#ifndef __CURVATURE_FLOW_H__
#define __CURVATURE_FLOW_H__

#include "FairingAlgorithm.h"

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>

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

		void set_delta(double delta);
		double get_delta();

		void set_steps(size_t num_steps);
		size_t get_steps();

		bool apply_to(mesh& input_mesh);

	private:
		boost::numeric::ublas::compressed_matrix<double> calc_curvature_operator(mesh& input_mesh);

		double dt;		///< Delta parameter when performing one step
		size_t num_steps;	///< Number of steps to perform
};

} // end of namespace "psalm"

#endif
