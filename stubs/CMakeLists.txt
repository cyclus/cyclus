# DO NOT DELETE THIS LINE:: @archetype_spec@

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
# taken from http://geant4.cern.ch/support/source/geant4/CMakeLists.txt
IF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(STATUS "Stub requires an out-of-source build.")
    message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
    message(STATUS "CMakeCache.txt")
    message(STATUS "CMakeFiles")
    message(STATUS "Once these files are removed, create a separate directory")
    message(STATUS "and run CMake from there")
    message(FATAL_ERROR "in-source build detected")
ENDIF()

PROJECT(@lib_name@)

# check for and enable c++11 support
INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
IF(COMPILER_SUPPORTS_CXX17)
    SET(CMAKE_CXX_STANDARD 17)
    SET(CMAKE_CXX_STANDARD_REQUIRED True)
    SET(CMAKE_CXX_EXTENSIONS OFF)
ELSE()
  MESSAGE(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11
  support. Please use a different C++ compiler.")
ENDIF()

# quiets fortify_source warnings when not compiling with optimizations
# in linux distros where compilers were compiled with fortify_source enabled by
# default (e.g. Arch linux).
MESSAGE("${CMAKE_BUILD_TYPE}")
STRING(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE)
IF(NOT ${BUILD_TYPE} STREQUAL "release")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0")
ENDIF()
# no overflow warnings because of silly coin-ness
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overflow")

# Direct any out-of-source builds to this directory
SET(STUB_SOURCE_DIR ${CMAKE_SOURCE_DIR})

# Direct any binary installation paths to this directory
SET(STUB_BINARY_DIR ${CMAKE_BINARY_DIR})

# This makes all the libraries build as SHARED
SET(BUILD_SHARED_LIBS true)

# Setup build locations.
IF(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${STUB_BINARY_DIR}/bin)
endif()
IF(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${STUB_BINARY_DIR}/lib)
endif()
IF(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${STUB_BINARY_DIR}/lib)
ENDIF()

SET(STUB_EXECUTABLE_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/lib/cyclus")
    GET_FILENAME_COMPONENT(cxxCompilerRoot ${CMAKE_CXX_COMPILER} DIRECTORY)
    GET_FILENAME_COMPONENT(cxxCompilerRoot ${cxxCompilerRoot} DIRECTORY)
    IF (NOT "${CMAKE_INSTALL_RPATH}" STREQUAL "${cxxCompilerRoot}")
        SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:${cxxCompilerRoot}/lib:${cxxCompilerRoot}/lib/cyclus")
    ENDIF (NOT "${CMAKE_INSTALL_RPATH}" STREQUAL "${cxxCompilerRoot}")
    SET(CMAKE_BUILD_RPATH "${CMAKE_INSTALL_RPATH}")
ENDIF("${isSystemDir}" STREQUAL "-1")

# Tell CMake where the modules are
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_INSTALL_PREFIX}/share/cyclus/cmake")
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${STUB_SOURCE_DIR}/cmake)
MESSAGE("--CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")

# Find cyclus
FIND_PACKAGE(Cyclus REQUIRED)
SET(STUB_INCLUDE_DIRS ${STUB_INCLUDE_DIRS} ${CYCLUS_CORE_INCLUDE_DIR})
SET(LIBS ${LIBS} ${CYCLUS_CORE_LIBRARIES})

# Include macros
INCLUDE(UseCyclus)

MESSAGE("--LD_LIBRARY_PATH: $ENV{LD_LIBRARY_PATH}")

# Include the boost header files, system, and filesystem libraries
SET(Boost_USE_STATIC_LIBS       OFF)
SET(Boost_USE_STATIC_RUNTIME    OFF)
FIND_PACKAGE(Boost COMPONENTS filesystem system REQUIRED)
SET(STUB_INCLUDE_DIRS ${STUB_INCLUDE_DIRS} ${Boost_INCLUDE_DIR})
SET(LIBS ${LIBS} ${Boost_FILESYSTEM_LIBRARY})
SET(LIBS ${LIBS} ${Boost_SYSTEM_LIBRARY})

# Find HDF5
FIND_PACKAGE(HDF5 REQUIRED)
ADD_DEFINITIONS(${HDF5_DEFINITIONS})
SET(STUB_INCLUDE_DIRS ${STUB_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR})
set(LIBS ${LIBS} ${HDF5_LIBRARIES})
MESSAGE("--    HDF5 Root: ${HDF5_ROOT}")
MESSAGE("--    HDF5 Include directory: ${HDF5_INCLUDE_DIR}")
MESSAGE("--    HDF5 Library directories: ${HDF5_LIBRARY_DIRS}")
MESSAGE("--    HDF5 Libraries: ${HDF5_LIBRARIES}")

# find Coin
FIND_PACKAGE(COIN REQUIRED)
SET(STUB_INCLUDE_DIRS ${STUB_INCLUDE_DIRS} ${COIN_INCLUDE_DIR})
set(LIBS ${LIBS} ${COIN_LIBRARIES})


# include all the directories we just found
INCLUDE_DIRECTORIES(${STUB_INCLUDE_DIRS})


# add the agents
ADD_SUBDIRECTORY(src)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
