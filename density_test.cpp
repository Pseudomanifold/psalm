#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include <utility>

#include <limits>

#include "v3ctor.h"
#include "SubdivisionAlgorithms/Liepa.h"

std::pair<double, double> area_and_density(psalm::mesh& M, double initial_area = 0.0)
{
	std::pair<double, double> result;

	result.first = initial_area;
	if(initial_area == 0.0)
	{
		for(size_t i = 0; i < M.num_faces(); i++)
		{
			psalm::face* f = M.get_face(i);
			v3ctor a = f->get_vertex(1)->get_position() - f->get_vertex(0)->get_position();
			v3ctor b = f->get_vertex(2)->get_position() - f->get_vertex(0)->get_position();

			result.first += 0.5*(a|b).length();
		}
	}
	result.second = M.num_vertices()/result.first;

	std::cerr	<< "(area, density) = " << "("
			<< std::fixed << std::setprecision(4) << result.first << ","
			<< std::fixed << std::setprecision(4) << result.second
			<< ")" << "\n";

	return(result);
}

int main(int argc, char* argv[])
{
	psalm::mesh M;
	psalm::Liepa liepa_scheme;

	if(argc == 1)
		return(0);

	for(int i = 2; i < argc; i++)
	{
		M.load(argv[i]);
		std::pair<double, double> area_density = area_and_density(M);

		double initial_area = area_density.first;
		double initial_density = area_density.second;

		// Ignore large density values -- it makes no sense to try to fit anything here
		if(initial_density > 5000)
			continue;

		for(int j = 0; j < 49; j++)
		{
			double alpha = j*0.1;

			M.load(argv[i]);

			liepa_scheme.set_alpha(alpha);
			liepa_scheme.apply_to(M);

			std::cout	<< (initial_density) << "\t"
					<< (initial_area) << "\t"
					<< area_and_density(M, initial_area).second << "\t"
					<< (alpha)
					<< std::endl;
		}

		std::cout << "\n\n";
	}

	return(0);

	/*

	//M.load("../Holes/Salmanassar3_Holes_w_Normals_HR_CLEAN_with_Indices.pline_0000_0008.ply");
	//std::cout << "INITIAL: " << area_and_density(M) << "\n";

	for(int i = 1; i <= 100; i++)
	{
		double alpha = i*0.05;

		M.load("../Holes/Salmanassar3_Holes_w_Normals_HR_CLEAN_with_Indices.pline_0000_0025.ply");

		liepa_scheme.set_alpha(alpha);
		liepa_scheme.apply_to(M);

		std::cout << (alpha) << "\t" << area_and_density(M) << std::endl;
	}

	//M.load("../Holes/Salmanassar3_Holes_w_Normals_HR_CLEAN_with_Indices.pline_0000_0008.ply");
	//liepa_scheme.set_alpha(sqrt(2));
	//liepa_scheme.apply_to(M);

	std::cout << "\n\n";
	for(int i = 1; i <= 100; i++)
	{
		double alpha = i*0.05;

		M.load("../Holes/Salmanassar3_Holes_w_Normals_HR_CLEAN_with_Indices.pline_0000_0008.ply");

		liepa_scheme.set_alpha(alpha);
		liepa_scheme.apply_to(M);

		std::cout << (alpha) << "\t" << area_and_density(M) << std::endl;
	}

	std::cout << "\n\n";
	for(int i = 1; i <= 100; i++)
	{
		double alpha = i*0.05;

		M.load("../Holes/Salmanassar3_Holes_w_Normals_HR_CLEAN_with_Indices.pline_0000_0009.ply");

		liepa_scheme.set_alpha(alpha);
		liepa_scheme.apply_to(M);

		std::cout << (alpha) << "\t" << area_and_density(M) << std::endl;
	}


	std::cout << "\n\n";
	for(int i = 1; i <= 100; i++)
	{
		double alpha = i*0.05;

		M.load("../Holes/Salmanassar3_Holes_w_Normals_HR_CLEAN_with_Indices.pline_0000_0010.ply");

		liepa_scheme.set_alpha(alpha);
		liepa_scheme.apply_to(M);

		std::cout << (alpha) << "\t" << area_and_density(M) << std::endl;
	}

	std::cout << "\n\n";
	for(int i = 1; i <= 100; i++)
	{
		double alpha = i*0.05;

		M.load("../Holes/Salmanassar3_Holes_w_Normals_HR_CLEAN_with_Indices.pline_0000_0011.ply");

		liepa_scheme.set_alpha(alpha);
		liepa_scheme.apply_to(M);

		std::cout << (alpha) << "\t" << area_and_density(M) << std::endl;
	}

	*/

	//std::cerr << "END: " << area_and_density(M) << "\n";
	return(0);
}
