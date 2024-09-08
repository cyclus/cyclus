# This file contains build procedures that are common to multiple
# builds in the cyclus ecosystem.  These macros are intended to
# provide convenience for build maintenance.

MACRO(cyclus_minimum_cmake_version VERSION_NUMBER)
    SET(${VERSION_NUMBER} "3.16")
ENDMACRO()

MACRO(cyclus_require_out_of_source_build)
    IF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
        MESSAGE(STATUS "Cyclus requires an out-of-source build.")
        MESSAGE(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
        MESSAGE(STATUS "CMakeCache.txt")
        MESSAGE(STATUS "CMakeFiles")
        MESSAGE(STATUS "Once these files are removed, create a separate directory")
        MESSAGE(STATUS "and run CMake from there")
        MESSAGE(FATAL_ERROR "in-source build detected")
    ENDIF()
ENDMACRO()

MACRO(cyclus_require_cxx_support)
    INCLUDE(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
    IF(COMPILER_SUPPORTS_CXX17)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    ELSE()
        MESSAGE(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
    ENDIF()
ENDMACRO()

MACRO(cyclus_setup_build_locations TARGET_DIR)
    IF(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${TARGET_DIR}/bin")
    endif()
    IF(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
        SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${TARGET_DIR}/lib")
    endif()
    IF(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
        SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${TARGET_DIR}/lib")
    ENDIF()
ENDMACRO()

MACRO(cyclus_set_rpath)
    # use, i.e. don't skip the full RPATH for the build tree
    SET(CMAKE_SKIP_BUILD_RPATH FALSE)

    # when building, don't use the install RPATH already
    # (but later on when installing)
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${CMAKE_INSTALL_PREFIX}/lib/cyclus")

    # add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    # the RPATH to be used when installing, but only if it's not a system directory
    LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
    IF("${isSystemDir}" STREQUAL "-1")
        SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${CMAKE_INSTALL_PREFIX}/lib/cyclus")
        GET_FILENAME_COMPONENT(cxxCompilerRoot ${CMAKE_CXX_COMPILER} DIRECTORY)
        GET_FILENAME_COMPONENT(cxxCompilerRoot ${cxxCompilerRoot} DIRECTORY)
        IF (NOT "${CMAKE_INSTALL_RPATH}" STREQUAL "${cxxCompilerRoot}")
            SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${cxxCompilerRoot}/lib;${cxxCompilerRoot}/lib/cyclus")
        ENDIF (NOT "${CMAKE_INSTALL_RPATH}" STREQUAL "${cxxCompilerRoot}")
    ENDIF("${isSystemDir}" STREQUAL "-1")
    MESSAGE("-- CMAKE_INSTALL_RPATH: ${CMAKE_INSTALL_RPATH}")
    if (APPLE)
      set(BUILD_WITH_INSTALL_NAME_DIR TRUE)
    endif()
ENDMACRO()
