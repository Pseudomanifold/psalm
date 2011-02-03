CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(psalm) 

# This is evil but currently necessary. Else, it would be impossible to build
# libpsalm _and_ psalm from the same CMake file.
CMAKE_POLICY(SET CMP0002 OLD)

ADD_DEFINITIONS(-Wall -pedantic -g -O11)

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

SET(SA_TEST_SRC
	sa_test.cpp
	mesh.cpp
	v3ctor.cpp
	vertex.cpp
	edge.cpp
	directed_edge.cpp
	face.cpp
)

SET(DENSITY_TEST_SRC
	density_test.cpp
	mesh.cpp
	v3ctor.cpp
	vertex.cpp
	edge.cpp
	directed_edge.cpp
	face.cpp
	SubdivisionAlgorithms/Liepa.cpp
	SubdivisionAlgorithms/SubdivisionAlgorithm.cpp
)

# Include boost::program_options into psalm

FIND_PACKAGE(Boost 1.42 COMPONENTS program_options)
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

ADD_SUBDIRECTORY(SubdivisionAlgorithms)
ADD_SUBDIRECTORY(TriangulationAlgorithms)

ADD_EXECUTABLE(psalm ${PSALM_SRC})
ADD_EXECUTABLE(pline_fill ${PLINE_FILL_SRC})
ADD_EXECUTABLE(sa_test ${SA_TEST_SRC})
ADD_EXECUTABLE(density_test ${DENSITY_TEST_SRC})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR} SubdivisionAlgorithms TriangulationAlgorithms)
TARGET_LINK_LIBRARIES(sa_test SubdivisionAlgorithms TriangulationAlgorithms)
TARGET_LINK_LIBRARIES(psalm SubdivisionAlgorithms ${Boost_LIBRARIES})

# Build `libpsalm_test` and `libpsalm` _without_ relying on any libraries; this
# makes inclusion of the library easier.

FILE(GLOB LIBPSALM_SRC SubdivisionAlgorithms/*.cpp TriangulationAlgorithms/*.cpp)

SET(LIBPSALM_SRC
	${LIBPSALM_SRC}
	libpsalm.cpp
	v3ctor.cpp
	mesh.cpp
	hole.cpp
	face.cpp
	edge.cpp
	vertex.cpp
	directed_edge.cpp
)

ADD_LIBRARY(psalm ${LIBPSALM_SRC})

SET(LIBPSALM_SRC
	${LIBPSALM_SRC}
	libpsalm_test.cpp
)
ADD_EXECUTABLE(libpsalm_test ${LIBPSALM_SRC})

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
