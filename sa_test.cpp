#include "SubdivisionAlgorithms/Loop.h"

int main(int argc, char* argv[])
{
	psalm::mesh M;
	M.load("../Meshes/Icosahedron.ply");

	psalm::Loop loop_scheme;
	for(size_t i = 0; i < 6; i++)
	{
		loop_scheme.apply_to(M);
	}
	M.save("../test.ply");
	return(0);
}
