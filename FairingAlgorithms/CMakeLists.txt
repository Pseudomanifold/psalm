SET(	FAIRING_ALGORITHMS_SRC
	FairingAlgorithm.cpp
	CurvatureFlow.cpp
)

ADD_LIBRARY(FairingAlgorithms SHARED ${FAIRING_ALGORITHMS_SRC})
TARGET_LINK_LIBRARIES(FairingAlgorithms umfpack)

# FIXME: Include path should be set by other means
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR} ../external /usr/include/suitesparse)
