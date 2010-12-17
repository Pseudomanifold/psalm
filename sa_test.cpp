#include "SubdivisionAlgorithms/Loop.h"
#include "SubdivisionAlgorithms/DooSabin.h"

int main(int argc, char* argv[])
{
	psalm::mesh M;

	// LOOP

	M.load("../Meshes/Icosahedron.ply");

	psalm::Loop loop_scheme;
	for(size_t i = 0; i < 6; i++)
	{
		loop_scheme.apply_to(M);
	}
	M.save("../test_loop.ply");

	// DS

	M.load("../Meshes/Icosahedron.ply");

	psalm::DooSabin doo_sabin_scheme;
	for(int i = 0; i < 6; i++)
	{
		doo_sabin_scheme.apply_to(M);
	}
	M.save("../test_doo_sabin.ply");

	return(0);
}
