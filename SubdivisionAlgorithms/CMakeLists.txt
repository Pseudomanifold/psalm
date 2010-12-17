SET(	SUBDIVISION_ALGORITHMS_SRC
	SubdivisionAlgorithm.cpp
	Loop.cpp
	DooSabin.cpp
	CatmullClark.cpp
	BsplineSubdivisionAlgorithm.cpp
)

ADD_LIBRARY(SubdivisionAlgorithms SHARED ${SUBDIVISION_ALGORITHMS_SRC})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
