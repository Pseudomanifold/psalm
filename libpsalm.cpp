/*!
*	@file	libpsalm.cpp
*	@brief	Exports a function for filling holes in meshes. Used by GigaMesh.
*	@author	Bastian Rieck <bastian.rieck@iwr.uni-heidelberg.de>
*/

#include <sstream>
#include <string>
#include <vector>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "libpsalm.h"
#include "mesh.h"

#include "SubdivisionAlgorithms/Liepa.h"
#include "TriangulationAlgorithms/MinimumWeightTriangulation.h"

/*!
*	Generates a filename by creating a UUID and attaching the extension
*	onto it. This function is used for debugging purposes in order to save
*	auxiliary results of the triangulation.
*
*	@param	extension File extension
*	@return UUID.extension as a filename
*/

std::string generate_filename(std::string extension = "ply")
{
	boost::uuids::uuid uuid = boost::uuids::random_generator()();

	std::stringstream converter;
	converter << uuid << "." << extension;

	return(converter.str());
}

/*!
*	Given a polygonal line described as a list of vertices, this function
*	triangulates the hole and subdivides it. Afterwards, the new data is
*	stored in some arrays given as parameters of the caller.
*
*	@param num_vertices	Number of vertices in polygonal line
*
*	@param vertex_IDs	List of vertex IDs, used to identify them. If
*				this parameter is NULL, the mesh class will
*				assign sequential vertex IDs, starting with 0.
*
*	@param coordinates	Array of vertex coordinates (size: 3*num_vertices)
*
*	@param scale_attributes	Array of scale attributes (size: num_vertices).
*				The scale attribute is the average length of all
*				edges incident on the vertex. This parameter is
*				used for the subdivision algorithm. If the pointer
*				is NULL, the scale attributes will not be initialized
*				but the algorithm will still work.
*
*	@param normals		Array of vertex normals (size: 3*num_vertices)
*
*	@param desired_density	Desired density value (approximate) for the
*				filled hole
*
*	@param num_new_vertices	Number of new vertices created by the algorithm
*
*	@param new_coordinates	Array of vertex coordinates created by the
*				algorithm (size: 3*num_new_vertices)
*
*	@param num_new_faces	Number of new faces created by the algorithm
*
*	@param new_vertex_IDs	List of vertex IDs for the new faces (size:
*				3*num_new_faces). Negative IDs signify that the
*				vertex is _not_ new.
*
*	The nomenclature here might be confusing: new_vertex_IDs contains 3
*	vertex IDs that describe a new face created by the algorithm.
*
*	@returns true if the hole could be filled, otherwise false
*/

bool fill_hole(	int num_vertices, long* vertex_IDs, double* coordinates, double* scale_attributes, double* normals, double desired_density,
		int* num_new_vertices, double** new_coordinates, int* num_new_faces, long** new_vertex_IDs)
{
	bool result = true;
	if(	num_vertices == 0		||
		coordinates == NULL		||
		num_new_vertices == NULL	||
		new_coordinates == NULL		||
		num_new_faces == NULL		||
		new_vertex_IDs == NULL)
		return(false);

	psalm::mesh M;

	psalm::Liepa liepa_algorithm;
	psalm::MinimumWeightTriangulation triangulation_algorithm;

	result = M.load_raw_data(	num_vertices,
					vertex_IDs,
					coordinates,
					scale_attributes,
					normals);

	if(!result)
	{
		std::cerr << "libpsalm: Data processing failed for fill_hole() failed" << std::endl;
		return(false);
	}

	result = (result && triangulation_algorithm.apply_to(M));				// step 1: triangulate the hole

	double density = M.get_density();
	if(density <= desired_density)
		liepa_algorithm.set_alpha(libpsalm::estimate_density(	density,		// step 2: estimate density based on input parameters
												// and the triangulation from step 1
									desired_density));
	else
		liepa_algorithm.set_alpha(1.0);

	result = (result && liepa_algorithm.apply_to(M));					// step 3: apply Liepa's subdivision scheme; density
												// parameter has been set in step 2

	if(result)
	{
		M.save_raw_data(num_new_vertices,
				new_coordinates,
				num_new_faces,
				new_vertex_IDs);

		#ifdef DEBUG
			M.save(generate_filename());
		#endif
	}

	// signal an error for the calling function
	else
	{
		*num_new_vertices	= 0;
		*num_new_faces		= 0;
	}

	return(result);
}

/*!
*	Given a mesh input density and the desired density of the triangulated
*	hole, calculates an estimate for the density parameter of Liepa's
*	subdivision scheme.
*
*	@param input_density	Average density of the input data
*	@param desired_density	Desired density of triangulated hole
*
*	@returns Estimated value for the alpha parameter of Liepa's subdivision
*	scheme.
*
*	@warning The estimation is only useful for density values < 2000. A
*	density much larger than this would require separate treatment.
*/

namespace libpsalm
{

double estimate_density(double input_density, double desired_density)
{
	// Fitted parameter values with asymptotic standard error of < 3%,
	// which is sufficient for most mesh data.


	double a0 =  7.63324e-07;
	double a1 = -0.00710062;
	double b0 = -4.70052e-07;
	double b1 =  0.00573126 ;
	double c0 =  2.29083;

	double x = input_density; // abbreviations so that the function is written more easily
	double y = desired_density;

	return(a0*x*x+a1*x+b0*y*y+b1*y+c0);
}

} // end of namespace "libpsalm"
