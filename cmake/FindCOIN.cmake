# agented after FindCOIN.cmake in the lemon project

# Written by: Matthew Gidden
# Last updated: 12/17/12
# Last updated: 16/08/12

# This cmake file is designed to locate coin-related
# dependencies on a filesystem.
#
# If the coin dependencies were installed in a non-standard
# directory, e.g. installed from source perhaps, then
# the user can provide a prefix hint via the COIN_ROOT_DIR
# cmake variable:
#       $> cmake ../src -DCOIN_ROOT_DIR=/path/to/coin/root

# To date, this install requires the following dev versions
# of the respective coin libraries:
#       * coinor-libCbc-dev
#       * coinor-libClp-dev
#       * coinor-libcoinutils-dev
#       * coinor-libOsi-dev

#
# Get the root directory hint if provided
#
IF(NOT DEFINED COIN_ROOT_DIR)
    SET(COIN_ROOT_DIR "$ENV{COIN_ROOT_DIR}")
    MESSAGE("\tCOIN Root Dir: ${COIN_INCLUDE_DIR}")
ENDIF(NOT DEFINED COIN_ROOT_DIR)
MESSAGE(STATUS "COIN_ROOT_DIR hint is : ${COIN_ROOT_DIR}")

#
# Find the path based on a required header file
#
MESSAGE(STATUS "Coin multiple library dependency status:")
FIND_PATH(COIN_INCLUDE_DIR coin/CbcModel.hpp
    HINTS "${COIN_INCLUDE_DIR}"
    HINTS "${COIN_ROOT_DIR}/include"
    ${DEPS_INCLUDE_HINTS}
    HINTS /usr/
    HINTS /usr/include/
    HINTS /usr/local/
    HINTS /usr/local/include/
    HINTS /usr/coin/
    HINTS /usr/coin-Cbc/
    HINTS /usr/local/coin/
    HINTS /usr/local/coin-Cbc/
    )
set(COIN_INCLUDE_DIR ${COIN_INCLUDE_DIR}/coin)
MESSAGE("\tCOIN Include Dir: ${COIN_INCLUDE_DIR}")

#
# Find all coin library dependencies
#
FIND_LIBRARY(COIN_CBC_LIBRARY
    NAMES Cbc libCbc #libCbc.so.0
    HINTS "${COIN_INCLUDE_DIR}/../../lib/"
    HINTS "${COIN_ROOT_DIR}/lib"
    ${DEPS_LIB_HINTS}
    )
MESSAGE("\tCOIN CBC: ${COIN_CBC_LIBRARY}")

FIND_LIBRARY(COIN_CBC_SOLVER_LIBRARY
    NAMES CbcSolver libCbcSolver libCbcSolver.so.0
    HINTS ${COIN_INCLUDE_DIR}/../../lib/
    HINTS "${COIN_ROOT_DIR}/lib"
    ${DEPS_LIB_HINTS}
    )
MESSAGE("\tCOIN CBC solver: ${COIN_CBC_SOLVER_LIBRARY}")

FIND_LIBRARY(COIN_CGL_LIBRARY
    NAMES Cgl libCgl libCgl.so.0
    HINTS ${COIN_INCLUDE_DIR}/../../lib/
    HINTS "${COIN_ROOT_DIR}/lib"
    ${DEPS_LIB_HINTS}
    )
MESSAGE("\tCOIN CGL: ${COIN_CGL_LIBRARY}")

FIND_LIBRARY(COIN_CLP_SOLVER_LIBRARY
    NAMES ClpSolver libClpSolver libClpSolver.so.0
    HINTS ${COIN_INCLUDE_DIR}/../../lib/
    HINTS "${COIN_ROOT_DIR}/lib"
    ${DEPS_LIB_HINTS}
    )
MESSAGE("\tCOIN CLP SOLVER: ${COIN_CLP_SOLVER_LIBRARY}")

FIND_LIBRARY(COIN_CLP_LIBRARY
    NAMES Clp libClp libClp.so.0
    HINTS ${COIN_INCLUDE_DIR}/../../lib/
    HINTS "${COIN_ROOT_DIR}/lib"
    ${DEPS_LIB_HINTS}
    )
MESSAGE("\tCOIN CLP: ${COIN_CLP_LIBRARY}")

FIND_LIBRARY(COIN_COIN_UTILS_LIBRARY
    NAMES CoinUtils libCoinUtils libCoinUtils.so.0
    HINTS ${COIN_INCLUDE_DIR}/../../lib/
    HINTS "${COIN_ROOT_DIR}/lib"
    ${DEPS_LIB_HINTS}
    )
MESSAGE("\tCOIN UTILS: ${COIN_COIN_UTILS_LIBRARY}")

FIND_LIBRARY(COIN_OSI_LIBRARY
    NAMES Osi libOsi libOsi.so.0
    HINTS ${COIN_INCLUDE_DIR}/../../lib/
    HINTS "${COIN_ROOT_DIR}/lib"
    ${DEPS_LIB_HINTS}
    )
MESSAGE("\tCOIN OSI: ${COIN_OSI_LIBRARY}")

FIND_LIBRARY(COIN_OSI_CBC_LIBRARY
    NAMES OsiCbc libOsiCbc #libOsiCbc.so.0
    HINTS ${COIN_INCLUDE_DIR}/../../lib/
    HINTS "${COIN_ROOT_DIR}/lib"
    ${DEPS_LIB_HINTS}
    )
MESSAGE("\tCOIN OSI CBC: ${COIN_OSI_CBC_LIBRARY}")

FIND_LIBRARY(COIN_OSI_CLP_LIBRARY
    NAMES OsiClp libOsiClp libOsiClp.so.0
    HINTS ${COIN_INCLUDE_DIR}/../../lib/
    HINTS "${COIN_ROOT_DIR}/lib"
    ${DEPS_LIB_HINTS}
    )
MESSAGE("\tCOIN OSI CLP: ${COIN_OSI_CLP_LIBRARY}")

FIND_LIBRARY(COIN_ZLIB_LIBRARY
    NAMES z libz libz.so.1
    HINTS ${COIN_ROOT_DIR}/lib
    HINTS "${COIN_ROOT_DIR}/lib"
    ${DEPS_LIB_HINTS}
    )
MESSAGE("\tCOIN ZLIB: ${COIN_ZLIB_LIBRARY}")

FIND_LIBRARY(COIN_BZ2_LIBRARY
    NAMES bz2 libz2 libz2.so.1
    HINTS ${COIN_ROOT_DIR}/lib
    HINTS "${COIN_ROOT_DIR}/lib"
    ${DEPS_LIB_HINTS}
    )
MESSAGE("\tCOIN BZ2: ${COIN_BZ2_LIBRARY}")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(COIN DEFAULT_MSG
    COIN_INCLUDE_DIR
    COIN_CBC_LIBRARY
    COIN_CBC_SOLVER_LIBRARY
    COIN_CGL_LIBRARY
    COIN_CLP_LIBRARY
    COIN_COIN_UTILS_LIBRARY
    COIN_OSI_LIBRARY
    # Not required by cbc v2.5, but required by later versions
    COIN_OSI_CBC_LIBRARY
    COIN_OSI_CLP_LIBRARY
    COIN_ZLIB_LIBRARY
    COIN_BZ2_LIBRARY
    )

#
# Set all required cmake variables based on our findings
#
IF(COIN_FOUND)
    SET(COIN_INCLUDE_DIRS ${COIN_INCLUDE_DIR})
    #SET(COIN_CLP_LIBRARIES "${COIN_CLP_LIBRARY};${COIN_COIN_UTILS_LIBRARY};${COIN_ZLIB_LIBRARY};${COIN_CLP_SOLVER_LIBRARY}")
    SET(COIN_CLP_LIBRARIES "${COIN_CLP_LIBRARY};${COIN_COIN_UTILS_LIBRARY};${COIN_ZLIB_LIBRARY}")
    IF (COIN_CLP_SOLVER_LIBRARY)
        SET(COIN_CLP_LIBRARIES "${COIN_CLP_LIBRARIES};${COIN_CLP_SOLVER_LIBRARY}")
    ENDIF (COIN_CLP_SOLVER_LIBRARY)
    IF(COIN_ZLIB_LIBRARY)
        SET(COIN_CLP_LIBRARIES "${COIN_CLP_LIBRARIES};${COIN_ZLIB_LIBRARY}")
    ENDIF(COIN_ZLIB_LIBRARY)
    IF(COIN_BZ2_LIBRARY)
        SET(COIN_CLP_LIBRARIES "${COIN_CLP_LIBRARIES};${COIN_BZ2_LIBRARY}")
    ENDIF(COIN_BZ2_LIBRARY)
    # Not required by cbc v2.5, but required by later versions in which case,
    # the lower line should be commented out and this line used
    #SET(COIN_CBC_LIBRARIES "${COIN_CBC_LIBRARY};${COIN_CBC_SOLVER_LIBRARY};${COIN_CGL_LIBRARY};${COIN_OSI_LIBRARY};${COIN_OSI_CBC_LIBRARY};${COIN_OSI_CLP_LIBRARY};${COIN_CLP_LIBRARIES}")
    SET(COIN_CBC_LIBRARIES "${COIN_OSI_LIBRARY};${COIN_CLP_LIBRARIES};${COIN_OSI_CLP_LIBRARY};${COIN_CGL_LIBRARY};${COIN_CBC_SOLVER_LIBRARY};${COIN_CBC_LIBRARY};${COIN_OSI_CBC_LIBRARY}")
    #SET(COIN_CBC_LIBRARIES "${COIN_CBC_LIBRARY};${COIN_CBC_SOLVER_LIBRARY};${COIN_CGL_LIBRARY};${COIN_OSI_LIBRARY};${COIN_OSI_CLP_LIBRARY};${COIN_CLP_LIBRARIES}")
    SET(COIN_LIBRARIES "${COIN_CBC_LIBRARIES}")

    FILE(STRINGS "${COIN_INCLUDE_DIR}/CbcConfig.h" COIN_VERSION REGEX "define CBC_VERSION .*")
    STRING(REPLACE "#define CBC_VERSION " "" COIN_VERSION "${COIN_VERSION}")
    STRING(REPLACE "\"" "" COIN_VERSION "${COIN_VERSION}")
ENDIF(COIN_FOUND)

#
# Report a synopsis of our findings
#
IF(COIN_INCLUDE_DIRS)
    MESSAGE(STATUS "Found COIN Include Dirs: ${COIN_INCLUDE_DIRS}")
ELSE(COIN_INCLUDE_DIRS)
    MESSAGE(STATUS "COIN Include Dirs NOT FOUND")
ENDIF(COIN_INCLUDE_DIRS)
