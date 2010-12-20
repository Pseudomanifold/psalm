/*!
*	@file	Liepa.h
*	@brief	Class describing Liepa's subdivision scheme
*/

#ifndef __LIEPA_H__
#define __LIEPA_H__

#include "SubdivisionAlgorithm.h"

namespace psalm
{

/*!
*	@class Liepa
*	@brief Liepa subdivision algorithm
*/

class Liepa : public SubdivisionAlgorithm
{
	public:
		Liepa();

		bool apply_to(mesh& input_mesh);

		void set_alpha(double alpha);
		double get_alpha();

	private:

		/*!
			User-configurable density parameter. Default value is
			set in constructor.
		*/

		double alpha;
};

} // end of namespace "psalm"

#endif
