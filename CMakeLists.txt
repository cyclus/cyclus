CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
#taken from http://geant4.cern.ch/support/source/geant4/CMakeLists.txt
IF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(STATUS "Cyclus requires an out-of-source build.")
    message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
    message(STATUS "CMakeCache.txt")
    message(STATUS "CMakeFiles")
    message(STATUS "Once these files are removed, create a separate directory")
    message(STATUS "and run CMake from there")
    message(FATAL_ERROR "in-source build detected")
ENDIF()

# This project name is cyclus.
PROJECT( CYCLUS )

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
MESSAGE("${CMAKE_BUILD_TYPE}")
STRING(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE)
IF(NOT ${BUILD_TYPE} STREQUAL "release")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0")
ENDIF()
# no overflow warnings because of silly coin-ness
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overflow")

IF(NOT CYCLUS_DOC_ONLY)

    # download pre-generated decay source code files from pyne
    EXECUTE_PROCESS(COMMAND python ${PROJECT_SOURCE_DIR}/getdecay.py --root ${PROJECT_SOURCE_DIR}
                    RESULT_VARIABLE res_var)

    IF(NOT "${res_var}" STREQUAL "0")
      message(FATAL_ERROR "fetching decay source code failed, res_var = '${res_var}'")
    ENDIF()

    ##############################################################################################
    ################################# begin cmake configuration ##################################
    ##############################################################################################

    # set project directories
    SET(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    SET(CYCLUS_SHARE_DIR "${PROJECT_SOURCE_DIR}/share")
    SET(CYCLUS_CLI_DIR "${PROJECT_SOURCE_DIR}/cli")
    SET(CYCLUS_DOC_DIR "${PROJECT_SOURCE_DIR}/doc")
    SET(CYCLUS_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
    SET(CYCLUS_STUB_DIR "${PROJECT_SOURCE_DIR}/stubs")
    SET(CYCLUS_TEST_DIR "${PROJECT_SOURCE_DIR}/tests")
    SET(CYCLUS_AGENTS_DIR "${PROJECT_SOURCE_DIR}/agents")
    SET(CYCLUS_CMAKE_DIR "${PROJECT_SOURCE_DIR}/cmake")

    # set cycpp var
    SET(CYCPP "${CYCLUS_CLI_DIR}/cycpp.py")

    # This makes all the libraries build as SHARED
    SET(BUILD_SHARED_LIBS true)

    # Setup build locations.
    IF(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CYCLUS_BINARY_DIR}/bin")
    endif()
    IF(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
        SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CYCLUS_BINARY_DIR}/lib")
    endif()
    IF(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
        SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CYCLUS_BINARY_DIR}/lib")
    ENDIF()

    SET(CYCLUS_EXECUTABLE_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

    # use, i.e. don't skip the full RPATH for the build tree
    SET(CMAKE_SKIP_BUILD_RPATH FALSE)

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
    ENDIF("${isSystemDir}" STREQUAL "-1")

    # Tell CMake where the modules are
    LIST(APPEND CMAKE_MODULE_PATH
        "${CMAKE_DIR}/share/cmake-2.8/Modules" "${PROJECT_SOURCE_DIR}/cmake")

    # Include macros
    INCLUDE(UseCyclus)

    # set configuration for all library builds
    IF (WIN32)
        set(suffix ".dll")
        set(dynamicloadlib "windows_helper_functions.h")
    ELSEIF(APPLE)
        set(CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup")
        set(suffix ".dylib")
        set(dynamicloadlib "unix_helper_functions.h")
    ELSE()
        set(suffix ".so")
        set(dynamicloadlib "unix_helper_functions.h")
    ENDIF()

    # enable testing, must be at top-level cmake file
    OPTION( USE_TESTING "Build testing" ON )
    IF( USE_TESTING )
        enable_testing()
        INCLUDE(CTest)
    ENDIF()

    ##############################################################################################
    ################################## end cmake configuration ###################################
    ##############################################################################################

    ##############################################################################################
    ################################### begin find libraries #####################################
    ##############################################################################################

    MESSAGE("-- Dependency Root Dir (DEPS_ROOT_DIR): ${DEPS_ROOT_DIR}")

    FIND_PACKAGE(Tcmalloc)
    SET(LIBS ${LIBS} ${Tcmalloc_LIBRARIES})

    # Debian installs useful LibXML2 files to /usr/include/libxml2/libxml
    # libxml2 is required for relaxng schema validation
    FIND_PACKAGE(LibXml2 REQUIRED HINTS "${DEPS_ROOT_DIR}")
    ADD_DEFINITIONS(${LIBXML2_DEFINITIONS})
    SET(LIBS ${LIBS} ${LIBXML2_LIBRARIES})

    # Find LibXML++ and dependencies
    FIND_PACKAGE(LibXML++ REQUIRED)
    SET(LIBS ${LIBS} ${LibXML++_LIBRARIES})

    # Find Sqlite3
    FIND_PACKAGE(Sqlite3 REQUIRED)
    SET(LIBS ${LIBS} ${SQLITE3_LIBRARIES})

    # Find HDF5
    FIND_PACKAGE(HDF5 REQUIRED)
    ADD_DEFINITIONS(${HDF5_DEFINITIONS})
    set(LIBS ${LIBS} ${HDF5_LIBRARIES} )
    MESSAGE("--    HDF5 Root: ${HDF5_ROOT}")
    MESSAGE("--    HDF5 Include directory: ${HDF5_INCLUDE_DIR}")
    MESSAGE("--    HDF5 Library directories: ${HDF5_LIBRARY_DIRS}")
    MESSAGE("--    HDF5 Libraries: ${HDF5_LIBRARIES}")

    # Include the boost header files and the program_options library
    # Please be sure to use Boost rather than BOOST.
    # Capitalization matters on some platforms
    SET(Boost_USE_STATIC_LIBS OFF)
    SET(Boost_USE_STATIC_RUNTIME OFF)
    # SET(Boost_USE_MULTITHREADED    OFF)
    FIND_PACKAGE( Boost 1.46.1 COMPONENTS program_options filesystem system serialization REQUIRED)
    MESSAGE("--    Boost Root: ${Boost_ROOT}")
    MESSAGE("--    Boost Include directory: ${Boost_INCLUDE_DIR}")
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
    FIND_PACKAGE( LAPACK REQUIRED )
    set(LIBS ${LIBS} ${LAPACK_LIBRARIES})
    MESSAGE("\tFound LAPACK Libraries: ${LAPACK_LIBRARIES}")

    # find coin and link to it
    FIND_PACKAGE( COIN REQUIRED )
    set(LIBS ${LIBS} ${COIN_LIBRARIES})

    #
    # Some optional libraries to link in, as availble. Required for conda.
    #
    # pcre
    FIND_LIBRARY(PCRE_LIBRARIES pcre
                 HINTS "${DEPS_ROOT_DIR}" "${DEPS_ROOT_DIR}/lib")
    MESSAGE("-- Found PCRE Libraries (optional): ${PCRE_LIBRARIES}")
    IF(PCRE_LIBRARIES)
        set(LIBS ${LIBS} ${PCRE_LIBRARIES})
    ENDIF(PCRE_LIBRARIES)

    # openblas
    FIND_LIBRARY(OPENBLAS_LIBRARIES openblas
                 HINTS "${DEPS_ROOT_DIR}" "${DEPS_ROOT_DIR}/lib")
    MESSAGE("-- Found OPENBLAS Libraries (optional): ${OPENBLAS_LIBRARIES}")
    IF(OPENBLAS_LIBRARIES)
        set(LIBS ${LIBS} ${OPENBLAS_LIBRARIES})
    ENDIF(OPENBLAS_LIBRARIES)

    # ClpSolver
    FIND_LIBRARY(CLPSOLVER_LIBRARIES ClpSolver
                 HINTS "${DEPS_ROOT_DIR}" "${DEPS_ROOT_DIR}/lib")
    MESSAGE("-- Found CLPSOLVER Libraries (optional): ${CLPSOLVER_LIBRARIES}")
    IF(CLPSOLVER_LIBRARIES)
        set(LIBS ${LIBS} ${CLPSOLVER_LIBRARIES})
    ENDIF(CLPSOLVER_LIBRARIES)

    # iconv
    FIND_LIBRARY(ICONV_LIBRARIES iconv
                 HINTS "${DEPS_ROOT_DIR}" "${DEPS_ROOT_DIR}/lib")
    MESSAGE("-- Found ICONV Libraries (optional): ${ICONV_LIBRARIES}")
    IF(ICONV_LIBRARIES)
        set(LIBS ${LIBS} ${ICONV_LIBRARIES})
    ENDIF(ICONV_LIBRARIES)

    # icudata
    FIND_LIBRARY(ICUDATA_LIBRARIES icudata
                 HINTS "${DEPS_ROOT_DIR}" "${DEPS_ROOT_DIR}/lib")
    MESSAGE("-- Found ICUDATA Libraries (optional): ${ICUDATA_LIBRARIES}")
    IF(ICUDATA_LIBRARIES)
        set(LIBS ${LIBS} ${ICUDATA_LIBRARIES})
    ENDIF(ICUDATA_LIBRARIES)

    # icui18n
    FIND_LIBRARY(ICUI18N_LIBRARIES icui18n
                 HINTS "${DEPS_ROOT_DIR}" "${DEPS_ROOT_DIR}/lib")
    MESSAGE("-- Found ICUI18N Libraries (optional): ${ICUI18N_LIBRARIES}")
    IF(ICUI18N_LIBRARIES)
        set(LIBS ${LIBS} ${ICUI18N_LIBRARIES})
    ENDIF(ICUI18N_LIBRARIES)

    # icuuc
    FIND_LIBRARY(ICUUC_LIBRARIES icuuc
                 HINTS "${DEPS_ROOT_DIR}" "${DEPS_ROOT_DIR}/lib")
    MESSAGE("-- Found ICUUC Libraries (optional): ${ICUUC_LIBRARIES}")
    IF(ICUUC_LIBRARIES)
        set(LIBS ${LIBS} ${ICUUC_LIBRARIES})
    ENDIF(ICUUC_LIBRARIES)

    ##############################################################################################
    #################################### end find libraries ######################################
    ##############################################################################################

    ##############################################################################################
    ###################################### begin includes ########################################
    ##############################################################################################

    # include all the directories we just found
    # NOTE: for some reason, adding quotes around
    # ${Glibmm_INCLUDE_DIRS} breaks Ubuntu 12.04
    INCLUDE_DIRECTORIES("${LIBXML2_INCLUDE_DIR}"
        "${LibXML++_INCLUDE_DIR}"
        "${Glibmm_INCLUDE_DIRS}"
        "${LibXML++Config_INCLUDE_DIR}"
        "${SQLITE3_INCLUDE_DIR}"
        "${HDF5_INCLUDE_DIRS}"
        "${Boost_INCLUDE_DIR}"
        "${COIN_INCLUDE_DIRS}")

    EXECUTE_PROCESS(COMMAND git describe --tags OUTPUT_VARIABLE core_version OUTPUT_STRIP_TRAILING_WHITESPACE)

    IF("${core_version}" STREQUAL "")
      MESSAGE(WARNING "Unable to read current core version, falling back to previous version. Core version will be set to -1")
      SET(core_version "-1")
    ENDIF()

    ADD_SUBDIRECTORY("${CYCLUS_SHARE_DIR}")
    ADD_SUBDIRECTORY("${CYCLUS_SOURCE_DIR}")
    ADD_SUBDIRECTORY("${CYCLUS_TEST_DIR}")
    ADD_SUBDIRECTORY("${CYCLUS_AGENTS_DIR}")
    ADD_SUBDIRECTORY("${CYCLUS_CLI_DIR}")
    ADD_SUBDIRECTORY("${CYCLUS_CMAKE_DIR}")

    ##############################################################################################
    ####################################### end includes #########################################
    ##############################################################################################

    ################# stub files #################################################################
    INSTALL(
        FILES
        stubs/CMakeLists.txt
        stubs/example_facility.xml
        stubs/example_inst.xml
        stubs/example_region.xml
        stubs/install.py
        stubs/README.rst
        DESTINATION share/cyclus/stubs
        COMPONENT stubs
        )

    INSTALL(
        FILES
            stubs/src/CMakeLists.txt
            stubs/src/stub_facility.h
            stubs/src/stub_facility.cc
            stubs/src/stub_facility_tests.cc
            stubs/src/stub_inst.h
            stubs/src/stub_inst.cc
            stubs/src/stub_inst_tests.cc
            stubs/src/stub_region.h
            stubs/src/stub_region.cc
            stubs/src/stub_region_tests.cc
            stubs/src/stub_unit_test_driver.cc
        DESTINATION share/cyclus/stubs/src
        COMPONENT stubs
        )
    ##############################################################################################
    ################################## begin uninstall target ####################################
    ##############################################################################################

    configure_file(
        "${PROJECT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
        )

    add_custom_target(uninstall
        COMMAND "${CMAKE_COMMAND}" -P "\"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake\"")

    ##############################################################################################
    ################################### end uninstall target #####################################
    ##############################################################################################

    ##############################################################################################
    ####################################### begin cpack ##########################################
    ##############################################################################################

    SET(CPACK_COMPONENTS_ALL cyclus testing libraries data core)

    # Here's where we package it with CPack
    SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cyclus : A nuclear fuel cycle simulator from UW-Madison.")

    SET(CPACK_GENERATOR "DEB")
    SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "CNERG, UW-Madison") #required
    SET(CPACK_PACKAGE_VENDOR "CNERG, UW-Madison")
    SET(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.rst")
    SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.rst")
    SET(CPACK_PACKAGE_VERSION_MAJOR "1")
    SET(CPACK_PACKAGE_VERSION_MINOR "0")
    #SET(CPACK_PACKAGE_VERSION_PATCH "0") # should use commit number here  ?
    SET(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
    SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY "1")
    SET(CPACK_PACKAGE_INSTALL_DIRECTORY "cyclus${CPACK_VERSION_MAJOR}.${CPACK_VERSION_MINOR}")
    #SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-all-dev  (>= 1.46.0-1), libxml++2.6-dev (>= 2.6.0), libsqlite3-dev (>= 3.7.9), libhdf5-serial-dev (>= 1.8.0)")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/cmake/CyclusCPackOptions.cmake.in"
        "${CYCLUS_BINARY_DIR}/cmake/CyclusCPackOptions.cmake" @ONLY)
    SET(CPACK_PROJECT_CONFIG_FILE "${CYCLUS_BINARY_DIR}/cmake/CyclusCPackOptions.cmake")
    SET(CPACK_PACKAGE_EXECUTABLES "cyclus" "CyclusUnitTestDriver")

    INCLUDE(CPack)

    ##############################################################################################
    ######################################## end cpack ###########################################
    ##############################################################################################

ENDIF(NOT CYCLUS_DOC_ONLY)

##############################################################################################
###################################### begin doxygen #########################################
##############################################################################################

# This is the directory that holds the doxygen doxyfile template (doxy.conf.in)
SET(DOC_SOURCE_DIR "${CYCLUS_SOURCE_DIR}")
SET(DOC_INPUT_DIR "${CYCLUS_DOC_DIR}")
SET(DOC_OUTPUT_DIR "${CMAKE_BINARY_DIR}/doc")

# If doxygen exists, use the doc/CMakeLists.txt file for further instructions.
FIND_PACKAGE(Doxygen)
IF(DOXYGEN_FOUND)
    ADD_SUBDIRECTORY("${CYCLUS_DOC_DIR}")
ELSE(DOXYGEN_FOUND)
    MESSAGE(STATUS "WARNING: Doxygen not found - doc won't be created")
ENDIF(DOXYGEN_FOUND)

##############################################################################################
####################################### end doxygen ##########################################
##############################################################################################
