INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
ADD_DEFINITIONS(-DDEBUG)

# `density_test`
SET(DENSITY_TEST_SRC
	density_test.cpp
	../mesh.cpp
	../v3ctor.cpp
	../vertex.cpp
	../edge.cpp
	../directed_edge.cpp
	../face.cpp
)

ADD_EXECUTABLE(density_test ${DENSITY_TEST_SRC})
TARGET_LINK_LIBRARIES(density_test SubdivisionAlgorithms TriangulationAlgorithms)

# `libpsalm_test`
SET(LIBPSALM_TEST_SRC
	libpsalm_test.cpp
	../libpsalm.cpp
	../v3ctor.cpp
	../mesh.cpp
	../face.cpp
	../edge.cpp
	../vertex.cpp
	../directed_edge.cpp
)

ADD_EXECUTABLE(libpsalm_test ${LIBPSALM_TEST_SRC})
TARGET_LINK_LIBRARIES(libpsalm_test SubdivisionAlgorithms TriangulationAlgorithms)
