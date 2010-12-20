#include <iostream>
#include <iomanip>
#include <cmath>

#include "v3ctor.h"
#include "SubdivisionAlgorithms/Liepa.h"

double area_and_density(psalm::mesh& M)
{
	double area = 0.0;
	for(size_t i = 0; i < M.num_faces(); i++)
	{
		psalm::face* f = M.get_face(i);
		for(size_t j = 0; j < 3; j++)
		{
			v3ctor a = f->get_vertex(1)->get_position() - f->get_vertex(0)->get_position();
			v3ctor b = f->get_vertex(2)->get_position() - f->get_vertex(0)->get_position();

			area += 0.5*(a|b).length();
		}
	}

	std::cerr	<< "(area, density) = " << "("
			<< std::fixed << std::setprecision(4) << area << ","
			<< std::fixed << std::setprecision(4) << (M.num_vertices()/area)
			<< ")" << "\n";

	return(M.num_vertices()/area);
}

int main(int argc, char* argv[])
{
	psalm::mesh M;
	psalm::Liepa liepa_scheme;

	M.load("../Holes/Salmanassar3_Holes_w_Normals_HR_CLEAN_with_Indices.pline_0000_0008.ply");
	std::cout << "INITIAL: " << area_and_density(M) << "\n";


	/*
	for(int i = 0; i <= 10; i++)
	{
		double alpha = i*3;

		M.load("../Holes/Salmanassar3_Holes_w_Normals_HR_CLEAN_with_Indices.pline_0000_0025.ply");

		liepa_scheme.set_alpha(alpha);
		liepa_scheme.apply_to(M);

		std::cout << (alpha) << "\t" << area_and_density(M) << std::endl;
	}
	*/

	M.load("../Holes/Salmanassar3_Holes_w_Normals_HR_CLEAN_with_Indices.pline_0000_0008.ply");
	liepa_scheme.set_alpha(sqrt(2));
	liepa_scheme.apply_to(M);

	std::cout << "END: " << area_and_density(M) << "\n";
	return(0);
}
