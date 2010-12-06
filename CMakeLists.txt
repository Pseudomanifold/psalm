CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(psalm) 

ADD_DEFINITIONS(-Wall -pedantic -g)

SET(PSALM_SRC
	psalm.cpp
	v3ctor.cpp
	mesh.cpp
	face.cpp
	vertex.cpp
	edge.cpp
	directed_edge.cpp
)

SET(PLINE_FILL_SRC
	pline_fill.cpp
	v3ctor.cpp
	mesh.cpp
	hole.cpp
	face.cpp
	vertex.cpp
	edge.cpp
	directed_edge.cpp
)

ADD_EXECUTABLE(psalm ${PSALM_SRC})
ADD_EXECUTABLE(pline_fill ${PLINE_FILL_SRC})

# Create subdivided test data using "meshlab"
ADD_CUSTOM_TARGET(	test
			echo "Catmull-Clark subdivision"
			./psalm -a cc -n 1 -o ICO_CC_01.ply ../Meshes/Icosahedron.ply
			./psalm -a cc -n 2 -o ICO_CC_02.ply ../Meshes/Icosahedron.ply
			./psalm -a cc -n 3 -o ICO_CC_03.ply ../Meshes/Icosahedron.ply
			echo "Doo-Sabin subdivision"
			./psalm -a ds -n 1 -o Meshes/ICO_DS_01.ply Meshes/Icosahedron.ply
			./psalm -a ds -n 2 -o Meshes/ICO_DS_02.ply Meshes/Icosahedron.ply
			./psalm -a ds -n 3 -o Meshes/ICO_DS_03.ply Meshes/Icosahedron.ply)

# Valgrind test run for memory leaks
FIND_PROGRAM(VALGRIND valgrind)
IF(VALGRIND)
	ADD_CUSTOM_TARGET(	memcheck
				${VALGRIND} --suppressions=psalm.supp --leak-check=full --show-reachable=yes --track-origins=yes ./psalm -n 2 Tetrahedron.ply
				rm Tetrahedron_subdivided.ply)
ENDIF(VALGRIND)

