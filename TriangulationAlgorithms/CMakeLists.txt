SET(	TRIANGULATION_ALGORITHMS_SRC
  TriangulationAlgorithm.cpp
  MinimumWeightTriangulation.cpp
	# Add mesh library dependency explicitly here in order to prevent
  # linker problems.
  ../directed_edge.cpp
  ../edge.cpp
  ../face.cpp
  ../mesh.cpp
  ../v3ctor.cpp
  ../vertex.cpp
)

ADD_LIBRARY(TriangulationAlgorithms SHARED ${TRIANGULATION_ALGORITHMS_SRC})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
