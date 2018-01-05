SET(	SUBDIVISION_ALGORITHMS_SRC
	BsplineSubdivisionAlgorithm.cpp
	SubdivisionAlgorithm.cpp
	CatmullClark.cpp
	DooSabin.cpp
	Liepa.cpp
	Loop.cpp
  # Add mesh library dependency explicitly here in order to prevent
  # linker problems.
  ../directed_edge.cpp
  ../edge.cpp
  ../face.cpp
  ../mesh.cpp
  ../v3ctor.cpp
  ../vertex.cpp
)

ADD_LIBRARY(SubdivisionAlgorithms SHARED ${SUBDIVISION_ALGORITHMS_SRC})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
