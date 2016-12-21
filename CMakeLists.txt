CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT( psalm ) 

ADD_DEFINITIONS(
  -Wall
  -Wextra
  -pedantic
  -O11
)

FIND_PACKAGE( Boost 1.42 COMPONENTS program_options )
LINK_DIRECTORIES( ${Boost_LIBRARY_DIRS} )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} )

ADD_SUBDIRECTORY( FairingAlgorithms )
ADD_SUBDIRECTORY( SegmentationAlgorithms )
ADD_SUBDIRECTORY( SubdivisionAlgorithms )
ADD_SUBDIRECTORY( TriangulationAlgorithms )

ADD_SUBDIRECTORY( Tests )

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}
  external
  #
  FairingAlgorithms
  SegmentationAlgorithms
  SubdivisionAlgorithms
  TriangulationAlgorithms
)

#
# Build psalm command-line interface; the target `psalm_cli` will be renamed
# later on. Otherwise, name clashes with libpsalm would occur.
#

SET( PSALM_SRC
  psalm.cpp
  v3ctor.cpp
  mesh.cpp
  face.cpp
  vertex.cpp
  edge.cpp
  directed_edge.cpp
)

ADD_EXECUTABLE( psalm_cli ${PSALM_SRC} )
TARGET_LINK_LIBRARIES( psalm_cli SubdivisionAlgorithms FairingAlgorithms SegmentationAlgorithms ${Boost_LIBRARIES} )
SET_TARGET_PROPERTIES( psalm_cli PROPERTIES OUTPUT_NAME psalm )

MESSAGE( STATUS ${Boost_LIBRARIES} )

#
# Build `libpsalm` _without_ relying on any libraries; this makes inclusion of
# the library easier.
#

SET( LIBPSALM_SRC
  libpsalm.cpp
  v3ctor.cpp
  mesh.cpp
  face.cpp
  edge.cpp
  vertex.cpp
  directed_edge.cpp
  #
  SubdivisionAlgorithms/Liepa.cpp
  SubdivisionAlgorithms/SubdivisionAlgorithm.cpp
  #
  TriangulationAlgorithms/MinimumWeightTriangulation.cpp
  TriangulationAlgorithms/TriangulationAlgorithm.cpp
)

ADD_LIBRARY( psalm ${LIBPSALM_SRC} )

#
# Create subdivided test data using "meshlab"
#

ADD_CUSTOM_TARGET( data_test
  echo "Catmull-Clark subdivision"
  ./psalm -a cc -n 1 -o ICO_CC_01.ply ../Meshes/Icosahedron.ply
  ./psalm -a cc -n 2 -o ICO_CC_02.ply ../Meshes/Icosahedron.ply
  ./psalm -a cc -n 3 -o ICO_CC_03.ply ../Meshes/Icosahedron.ply
  echo "Doo-Sabin subdivision"
  ./psalm -a ds -n 1 -o Meshes/ICO_DS_01.ply Meshes/Icosahedron.ply
  ./psalm -a ds -n 2 -o Meshes/ICO_DS_02.ply Meshes/Icosahedron.ply
  ./psalm -a ds -n 3 -o Meshes/ICO_DS_03.ply Meshes/Icosahedron.ply
)


#
# Valgrind test run for memory leaks
#

FIND_PROGRAM(VALGRIND valgrind)

IF(VALGRIND)
  ADD_CUSTOM_TARGET( memcheck
    ${VALGRIND} --suppressions=psalm.supp --leak-check=full --show-reachable=yes --track-origins=yes ./psalm -n 2 Tetrahedron.ply
    rm Tetrahedron_subdivided.ply)
ENDIF(VALGRIND)
