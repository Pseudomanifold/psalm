SET(	FAIRING_ALGORITHMS_SRC
  FairingAlgorithm.cpp
)

IF( EXISTS ${Boost_INCLUDE_DIRS}/boost/numeric/bindings/traits/ublas_sparse.hpp)
  LIST( APPEND FAIRING_ALGORITHMS_SRC
    CurvatureFlow.cpp
  )
ENDIF()

ADD_LIBRARY(FairingAlgorithms SHARED ${FAIRING_ALGORITHMS_SRC})

# FIXME: This is not optimal
IF( EXISTS ${Boost_INCLUDE_DIRS}/boost/numeric/bindings/traits/ublas_sparse.hpp)
  TARGET_LINK_LIBRARIES(FairingAlgorithms umfpack)
ENDIF()

# FIXME: Include path should be set by other means
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR} ../external /usr/include/suitesparse)
