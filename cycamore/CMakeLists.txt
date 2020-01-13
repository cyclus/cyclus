CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
#taken from http://geant4.cern.ch/support/source/geant4/CMakeLists.txt
IF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(STATUS "Cycamore requires an out-of-source build.")
    message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
    message(STATUS "CMakeCache.txt")
    message(STATUS "CMakeFiles")
    message(STATUS "Once these files are removed, create a separate directory")
    message(STATUS "and run CMake from there")
    message(FATAL_ERROR "in-source build detected")
ENDIF()

# This project name is cycamore.
PROJECT(CYCAMORE)

# check for and enable c++11 support (required for cyclus)
INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
IF(COMPILER_SUPPORTS_CXX11)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ELSE()
    MESSAGE(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
ENDIF()

# quiets fortify_source warnings when not compiling with optimizations
# in linux distros where compilers were compiled with fortify_source enabled by
# default (e.g. Arch linux).
MESSAGE("-- CMake Build Type: ${CMAKE_BUILD_TYPE}")
STRING(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE)
IF(NOT ${BUILD_TYPE} STREQUAL "release")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0")
ENDIF()
# no overflow warnings because of silly coin-ness
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overflow")

# Direct any out-of-source builds to this directory
SET(CYCAMORE_SOURCE_DIR ${CMAKE_SOURCE_DIR})

IF(NOT CYCLUS_DOC_ONLY)
    # Direct any binary installation paths to this directory
    SET(CYCAMORE_BINARY_DIR ${CMAKE_BINARY_DIR})

    # This makes all the libraries build as SHARED
    SET(BUILD_SHARED_LIBS true)

    # Setup build locations.
    IF(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CYCAMORE_BINARY_DIR}/bin)
    endif()
    IF(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
        SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CYCAMORE_BINARY_DIR}/lib)
    endif()
    IF(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
        SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CYCAMORE_BINARY_DIR}/lib)
    ENDIF()

    SET(CYCAMORE_EXECUTABLE_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

    # use, i.e. don't skip the full RPATH for the build tree
    SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

    # when building, don't use the install RPATH already
    # (but later on when installing)
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib:${CMAKE_INSTALL_PREFIX}/lib/cyclus")

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
    MESSAGE("-- CMAKE_INSTALL_RPATH: ${CMAKE_INSTALL_RPATH}")

    # Tell CMake where the modules are
    SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_INSTALL_PREFIX}/share/cyclus/cmake")
    SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CYCAMORE_SOURCE_DIR}/cmake)
    MESSAGE("--CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")

    # get dependency hints
    if (NOT DEPS_ROOT_DIR)
        get_filename_component(compdir ${CMAKE_CXX_COMPILER} DIRECTORY)
        get_filename_component(DEPS_ROOT_DIR ${compdir} DIRECTORY)
    endif (NOT DEPS_ROOT_DIR)
    SET(DEPS_HINTS HINTS "${DEPS_ROOT_DIR}")
    SET(DEPS_BIN_HINTS ${DEPS_HINTS} "${DEPS_ROOT_DIR}/bin")
    SET(DEPS_LIB_HINTS ${DEPS_HINTS} "${DEPS_ROOT_DIR}/lib")
    SET(DEPS_INCLUDE_HINTS HINTS "${DEPS_ROOT_DIR}/include")
    MESSAGE("-- Dependency Root Dir (DEPS_ROOT_DIR): ${DEPS_ROOT_DIR}")
    MESSAGE("-- Dependency Hints (DEPS_HINTS): ${DEPS_HINTS}")
    MESSAGE("-- Dependency Binary Hints (DEPS_BIN_HINTS): ${DEPS_BIN_HINTS}")
    MESSAGE("-- Dependency Library Hints (DEPS_LIB_HINTS): ${DEPS_LIB_HINTS}")
    MESSAGE("-- Dependency Include Hints (DEPS_INCLUDE_HINTS): ${DEPS_INCLUDE_HINTS}")

    # Find cyclus
    FIND_PACKAGE(Cyclus REQUIRED)
    SET(
      CYCAMORE_INCLUDE_DIRS
      ${CYCAMORE_INCLUDE_DIRS}
      ${CYCLUS_CORE_INCLUDE_DIR} # INSTALL_PATH/include/cyclus
      )
    SET(LIBS ${LIBS} ${CYCLUS_CORE_LIBRARIES})

    # Include macros installed with cyclus core
    INCLUDE(UseCyclus)

    # Find LibXML++ and dependencies
    FIND_PACKAGE(LibXML++)
    IF(NOT LibXML++_LIBRARIES)
        FIND_LIBRARY(LibXML++ REQUIRED ${DEPS_HINTS})
    ENDIF()
    SET(CYCAMORE_INCLUDE_DIRS ${CYCAMORE_INCLUDE_DIRS} ${LibXML++_INCLUDE_DIR} ${Glibmm_INCLUDE_DIRS} ${LibXML++Config_INCLUDE_DIR})
    SET(LIBS ${LIBS} ${LibXML++_LIBRARIES})
    MESSAGE("-- LIBS: ${LIBS}")

    MESSAGE("-- LD_LIBRARY_PATH: $ENV{LD_LIBRARY_PATH}")

    # Include the boost header files and the program_options library
    # Please be sure to use Boost rather than BOOST.
    # Capitalization matters on some platforms
    SET(Boost_USE_STATIC_LIBS OFF)
    SET(Boost_USE_STATIC_RUNTIME OFF)
    # SET(Boost_USE_MULTITHREADED    OFF)
    if (DEPS_ROOT_DIR)
      SET(BOOST_ROOT "${DEPS_ROOT_DIR}")
      SET(BOOST_INCLUDEDIR "${DEPS_INCLUDE_HINTS}")
    endif (DEPS_ROOT_DIR)
    FIND_PACKAGE(Boost COMPONENTS program_options filesystem system serialization REQUIRED)
    MESSAGE("--    Boost Root: ${Boost_ROOT}")
    MESSAGE("--    Boost Include directory: ${Boost_INCLUDE_DIR}")
    SET(CYCAMORE_INCLUDE_DIRS ${CYCAMORE_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
    MESSAGE("--    Boost Library directories: ${Boost_LIBRARY_DIRS}")
    SET(LIBS ${LIBS} ${Boost_PROGRAM_OPTIONS_LIBRARY})
    MESSAGE("--    Boost Program Options location: ${Boost_PROGRAM_OPTIONS_LIBRARY}")
    SET(LIBS ${LIBS} ${Boost_SYSTEM_LIBRARY})
    MESSAGE("--    Boost System location: ${Boost_SYSTEM_LIBRARY}")
    SET(LIBS ${LIBS} ${Boost_FILESYSTEM_LIBRARY})
    MESSAGE("--    Boost Filesystem location: ${Boost_FILESYSTEM_LIBRARY}")
    SET(LIBS ${LIBS} ${Boost_SERIALIZATION_LIBRARY})
    MESSAGE("--    Boost Serialization location: ${Boost_SERIALIZATION_LIBRARY}")

    # find lapack and link to it
    # note there is no include directory variable:
    # http://www.cmake.org/cmake/help/v3.0/module/FindLAPACK.html
    FIND_PACKAGE(LAPACK REQUIRED)
    set(LIBS ${LIBS} ${LAPACK_LIBRARIES})
    MESSAGE("\tFound LAPACK Libraries: ${LAPACK_LIBRARIES}")

    # Find HDF5
    FIND_PACKAGE(HDF5 REQUIRED COMPONENTS HL)
    ADD_DEFINITIONS(${HDF5_DEFINITIONS})
    if(NOT HDF5_LIBRARY_DIRS STREQUAL "")
      set(LIBS ${LIBS} ${HDF5_LIBRARIES})
      link_directories(${HDF5_LIBRARY_DIRS})
    endif()
    set(LIBS ${LIBS} ${HDF5_C_LIBRARIES} ${HDF5_C_HL_LIBRARIES})
    MESSAGE("--    HDF5 Root: ${HDF5_ROOT}")
    MESSAGE("--    HDF5 Include directory: ${HDF5_INCLUDE_DIR}")
    SET(CYCAMORE_INCLUDE_DIRS ${CYCAMORE_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR})
    MESSAGE("--    HDF5 Library directories: ${HDF5_LIBRARY_DIRS}")
    MESSAGE("--    HDF5 Libraries: ${HDF5_C_LIBRARIES}")
    MESSAGE("--    HDF5 High Level Libraries: ${HDF5_C_HL_LIBRARIES}")

    # find coin and link to it
    FIND_PACKAGE(COIN)
    if(COIN_FOUND)
      SET(CYCAMORE_INCLUDE_DIRS ${CYCAMORE_INCLUDE_DIRS} ${COIN_INCLUDE_DIRS})
      SET(LIBS ${LIBS} ${COIN_LIBRARIES})
    endif()
    MESSAGE("--    COIN Root: ${COIN_ROOT}")
    MESSAGE("--    COIN Include directories: ${COIN_INCLUDE_DIRS}")
    MESSAGE("--    COIN Libraries: ${COIN_LIBRARIES}")

    FIND_PACKAGE( Sqlite3 REQUIRED )
    SET(CYCAMORE_INCLUDE_DIRS ${CYCAMORE_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIR})
    SET(LIBS ${LIBS} ${SQLITE3_LIBRARIES})
    MESSAGE("--    SQLITE3 Include directories: ${SQLITE3_INCLUDE_DIR}")
    MESSAGE("--    SQLITE3 Libraries: ${SQLITE3_LIBRARIES}")

    #
    # Some optional libraries to link in, as availble. Required for conda.
    #
    # pcre
    FIND_LIBRARY(PCRE_LIBRARIES pcre ${DEPS_LIB_HINTS})
    MESSAGE("-- Found PCRE Libraries (optional): ${PCRE_LIBRARIES}")
    IF(PCRE_LIBRARIES)
        set(LIBS ${LIBS} ${PCRE_LIBRARIES})
    ENDIF(PCRE_LIBRARIES)

    # gfortran
    FIND_LIBRARY(GFORTRAN_LIBRARIES gfortran ${DEPS_LIB_HINTS})
    MESSAGE("-- Found GFORTRAN Libraries (optional): ${GFORTRAN_LIBRARIES}")
    IF(GFORTRAN_LIBRARIES)
        set(LIBS ${LIBS} ${GFORTRAN_LIBRARIES})
    ENDIF(GFORTRAN_LIBRARIES)

    # openblas
    FIND_LIBRARY(OPENBLAS_LIBRARIES openblas ${DEPS_LIB_HINTS})
    MESSAGE("-- Found OPENBLAS Libraries (optional): ${OPENBLAS_LIBRARIES}")
    IF(OPENBLAS_LIBRARIES)
        set(LIBS ${LIBS} ${OPENBLAS_LIBRARIES})
    ENDIF(OPENBLAS_LIBRARIES)

    # ClpSolver
    FIND_LIBRARY(CLPSOLVER_LIBRARIES ClpSolver ${DEPS_LIB_HINTS})
    MESSAGE("-- Found CLPSOLVER Libraries (optional): ${CLPSOLVER_LIBRARIES}")
    IF(CLPSOLVER_LIBRARIES)
        set(LIBS ${LIBS} ${CLPSOLVER_LIBRARIES})
    ENDIF(CLPSOLVER_LIBRARIES)

    # iconv
    FIND_LIBRARY(ICONV_LIBRARIES iconv ${DEPS_LIB_HINTS})
    MESSAGE("-- Found ICONV Libraries (optional): ${ICONV_LIBRARIES}")
    IF(ICONV_LIBRARIES)
        set(LIBS ${LIBS} ${ICONV_LIBRARIES})
    ENDIF(ICONV_LIBRARIES)

    # icudata
    FIND_LIBRARY(ICUDATA_LIBRARIES icudata ${DEPS_LIB_HINTS})
    MESSAGE("-- Found ICUDATA Libraries (optional): ${ICUDATA_LIBRARIES}")
    IF(ICUDATA_LIBRARIES)
        set(LIBS ${LIBS} ${ICUDATA_LIBRARIES})
    ENDIF(ICUDATA_LIBRARIES)

    # icui18n
    FIND_LIBRARY(ICUI18N_LIBRARIES icui18n ${DEPS_LIB_HINTS})
    MESSAGE("-- Found ICUI18N Libraries (optional): ${ICUI18N_LIBRARIES}")
    IF(ICUI18N_LIBRARIES)
        set(LIBS ${LIBS} ${ICUI18N_LIBRARIES})
    ENDIF(ICUI18N_LIBRARIES)

    # icuuc
    FIND_LIBRARY(ICUUC_LIBRARIES icuuc ${DEPS_LIB_HINTS})
    MESSAGE("-- Found ICUUC Libraries (optional): ${ICUUC_LIBRARIES}")
    IF(ICUUC_LIBRARIES)
        set(LIBS ${LIBS} ${ICUUC_LIBRARIES})
    ENDIF(ICUUC_LIBRARIES)

    # Python
    find_package(PythonInterp)
    message("-- PYTHON_EXECUTABLE: ${PYTHON_EXECUTABLE}")
    find_package(PythonLibs)
    MESSAGE("-- Found Python Libraries (optional): ${PYTHON_LIBRARIES}")
    IF(PYTHON_LIBRARIES)
        set(LIBS ${LIBS} ${PYTHON_LIBRARIES})
    ENDIF(PYTHON_LIBRARIES)

    # include all the directories we just found
    INCLUDE_DIRECTORIES(${CYCAMORE_INCLUDE_DIRS})

    # ------------------------- Add the Agents -----------------------------------
    ADD_SUBDIRECTORY(src)

    # ------------------------- Google Test -----------------------------------

    # Be sure to clear these each time
    SET(GENERATED_TEST_LIST "" CACHE INTERNAL "")
    SET(GENERATED_FILTER_LIST "" CACHE INTERNAL "")
    SET(GENERATED_TEST_SOURCE_LIST "" CACHE INTERNAL "")

    # Create an executable for all the gtests
    # Options for testing
    OPTION(USE_TESTING "Build testing" ON)
    IF(USE_TESTING)
        enable_testing()
        INCLUDE(CTest)
    ENDIF()

    ADD_SUBDIRECTORY(tests)

    SET(TestSource
        ${TestSource}
        ${CYCAMORE_TEST_CORE}
        )

    FIND_PACKAGE(Threads)
    IF(CMAKE_USE_PTHREADS_INIT)  # The pthreads library is available.
        SET(cxx_base_flags "${cxx_base_flags} -DGTEST_HAS_PTHREAD=1")
    ENDIF()

    # Build cycamore_unit_tests
    ADD_EXECUTABLE(cycamore_unit_tests
        tests/cycamore_unit_test_driver.cc
        ${TestSource}
        )

    TARGET_LINK_LIBRARIES(cycamore_unit_tests
        dl
        ${LIBS}
        cycamore
        ${CYCLUS_TEST_LIBRARIES}
        )

    INSTALL(TARGETS cycamore_unit_tests
        RUNTIME DESTINATION bin
        COMPONENT testing
        )

    ##############################################################################################
    ################################## begin uninstall target ####################################
    ##############################################################################################

    CONFIGURE_FILE(
        "${CMAKE_CURRENT_SOURCE_DIR}/config/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
        )

    ADD_CUSTOM_TARGET(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

    ##############################################################################################
    ################################### end uninstall target #####################################
    ##############################################################################################

    ##############################################################################################
    ####################################### begin cpack ##########################################
    ##############################################################################################


    # Here's where we package it with CPack
    SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cycamore : The Cyclus Additional Module Repository.")

    # Here we set some components for installation with cpack
    SET(CPACK_COMPONENTS_ALL cycamore testing libraries data core)
    SET(CPACK_GENERATOR "DEB")

    # Organisation
    SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "CNERG, UW-Madison") #required
    SET(CPACK_PACKAGE_VENDOR "CNERG, UW-Madison")
    SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.rst")
    SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.rst")

    # Version
    SET(CPACK_PACKAGE_VERSION_MAJOR "1")
    SET(CPACK_PACKAGE_VERSION_MINOR "4")
    SET(CPACK_PACKAGE_VERSION_PATCH "0")

    # Dependencies
    SET(CPACK_DEBIAN_PACKAGE_DEPENDS "cyclus (>= 1.4.0)")
    MESSAGE("CPACK_DEBIAN_PACKAGE_DEPENDS ${CPACK_DEBIAN_PACKAGE_DEPENDS}")

    # Names
    SET(CPACK_PACKAGE_NAME "cycamore")
    SET(CPACK_PACKAGE_INSTALL_DIRECTORY "cycamore_${CPACK_PACKAGE_VERSION_MAJOR}")
    SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_INSTALL_DIRECTORY}.${CPACK_PACKAGE_VERSION_MINOR}")
    SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_INSTALL_DIRECTORY}.${CPACK_PACKAGE_VERSION_PATCH}")

    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}")
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}_${CPACK_PACKAGE_VERSION_MAJOR}")
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}.${CPACK_PACKAGE_VERSION_MINOR}")
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}.${CPACK_PACKAGE_VERSION_PATCH}")
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}_${CPACK_HASH_NAME}")
    SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}_${CPACK_SYSTEM_NAME}")

    # Configuration
    SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY "1")
    SET(CPACK_DEBIAN_PACKAGE_SHLIBDEPS OFF)
    CONFIGURE_FILE(
      "${CYCAMORE_SOURCE_DIR}/cmake/CycamoreCPackOptions.cmake.in"
      "${CYCAMORE_BINARY_DIR}/cmake/CycamoreCPackOptions.cmake" @ONLY
      )
    SET(CPACK_PROJECT_CONFIG_FILE "${CYCAMORE_BINARY_DIR}/cmake/CycamoreCPackOptions.cmake")
    SET(CPACK_PACKAGE_EXECUTABLES "cycamore" "cycamore_unit_tests")

    INCLUDE(CPack)

    SET(ROOT_DIR ${CYCLUS_ROOT_DIR})
    SET(PROJ_DIR ${PROJECT_BINARY_DIR})
    SET(CORE_SHARE_DIR ${CYCLUS_CORE_SHARE_DIR})

    ##############################################################################################
    ######################################## end cpack ###########################################
    ##############################################################################################

ENDIF(NOT CYCLUS_DOC_ONLY)

# This is the directory that holds the doxygen doxyfile template (doxy.conf.in)
SET(DOC_INPUT_DIR ${CYCAMORE_SOURCE_DIR}/doc)

# If doxygen exists, use the doc/CMakeLists.txt file for further instructions.
FIND_PACKAGE(Doxygen)
IF(DOXYGEN_FOUND)
    ADD_SUBDIRECTORY(doc)
    SET(DOC_OUTPUT_DIR ${CMAKE_BINARY_DIR}/doc)
ELSE(DOXYGEN_FOUND)
    MESSAGE(STATUS "WARNING: Doxygen not found - doc won't be created")
ENDIF(DOXYGEN_FOUND)
