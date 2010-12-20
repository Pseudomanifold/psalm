SET(	SUBDIVISION_ALGORITHMS_SRC
	BsplineSubdivisionAlgorithm.cpp
	SubdivisionAlgorithm.cpp
	CatmullClark.cpp
	DooSabin.cpp
	Liepa.cpp
	Loop.cpp
)

ADD_LIBRARY(SubdivisionAlgorithms SHARED ${SUBDIVISION_ALGORITHMS_SRC})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
