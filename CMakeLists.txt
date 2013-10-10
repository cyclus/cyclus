# This project name is cyclus.
PROJECT( CYCLUS )
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

IF(NOT CYCLUS_DOC_ONLY)

##############################################################################################
################################# begin cmake configuration ##################################
##############################################################################################

# set project directories
SET( PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
SET( CYCLUS_CLI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cli )
SET( CYCLUS_DOC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/doc )
SET( CYCLUS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src )
SET( CYCLUS_STUB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/stubs )
SET( CYCLUS_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests )

# This makes all the libraries build as SHARED
SET(BUILD_SHARED_LIBS true)

# Setup build locations.
IF(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CYCLUS_BINARY_DIR}/bin)
endif()
IF(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CYCLUS_BINARY_DIR}/lib)
endif()
IF(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CYCLUS_BINARY_DIR}/lib)
ENDIF()

SET(CYCLUS_EXECUTABLE_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

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
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")

# Tell CMake where the modules are
SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_DIR}/share/cmake-2.8/Modules")
SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/CMake )

# Include macros
INCLUDE(CyclusModuleMacros)

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

# Debian installs useful LibXML2 files to /usr/include/libxml2/libxml
# libxml2 is required for relaxng schema validation
FIND_PACKAGE( LibXml2 REQUIRED )
SET(CYCLUS_INCLUDE_DIR ${CYCLUS_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR})
ADD_DEFINITIONS (${LIBXML2_DEFINITIONS})
SET(LIBS ${LIBS} ${LIBXML2_LIBRARIES})

# Find LibXML++ and dependencies
FIND_PACKAGE( LibXML++ REQUIRED )
SET(CYCLUS_INCLUDE_DIR ${CYCLUS_INCLUDE_DIR} ${LibXML++_INCLUDE_DIR} ${Glibmm_INCLUDE_DIRS} ${LibXML++Config_INCLUDE_DIR})
SET(LIBS ${LIBS} ${LibXML++_LIBRARIES})
MESSAGE(${LIBS})

# Find Sqlite3
FIND_PACKAGE( Sqlite3 REQUIRED )
SET(CYCLUS_INCLUDE_DIR ${CYCLUS_INCLUDE_DIR} ${SQLITE3_INCLUDE_DIR})
SET(LIBS ${LIBS} ${SQLITE3_LIBRARIES})

# Find HDF5
FIND_PACKAGE( HDF5 REQUIRED)
SET(CYCLUS_INCLUDE_DIR ${CYCLUS_INCLUDE_DIR} ${HDF5_INCLUDE_DIRS})
ADD_DEFINITIONS(${HDF5_DEFINITIONS})
set(LIBS ${LIBS} ${HDF5_LIBRARIES} )

# Include the boost header files and the program_options library
# Please be sure to use Boost rather than BOOST. 
# Capitalization matters on some platforms
SET(Boost_USE_STATIC_LIBS       OFF)
SET(Boost_USE_STATIC_RUNTIME    OFF)
# SET(Boost_USE_MULTITHREADED    OFF)
FIND_PACKAGE( Boost COMPONENTS program_options filesystem system REQUIRED)
SET(CYCLUS_INCLUDE_DIR ${CYCLUS_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
MESSAGE("--    Boost Root: ${Boost_ROOT}")
MESSAGE("--    Boost Include directory: ${Boost_INCLUDE_DIR}")
MESSAGE("--    Boost Library directories: ${Boost_LIBRARY_DIRS}")
SET(LIBS ${LIBS} ${Boost_PROGRAM_OPTIONS_LIBRARY})
MESSAGE("--    Boost Program Options location: ${Boost_PROGRAM_OPTIONS_LIBRARY}")
SET(LIBS ${LIBS} ${Boost_SYSTEM_LIBRARY})
MESSAGE("--    Boost System location: ${Boost_SYSTEM_LIBRARY}")
SET(LIBS ${LIBS} ${Boost_FILESYSTEM_LIBRARY})
MESSAGE("--    Boost Filesystem location: ${Boost_FILESYSTEM_LIBRARY}")

# find lapack and link to it
FIND_PACKAGE( LAPACK REQUIRED )
set(LIBS ${LIBS} ${LAPACK_LIBRARIES})
MESSAGE("\tFound LAPACK Libraries: ${LAPACK_LIBRARIES}")

# find coin and link to it
FIND_PACKAGE( COIN REQUIRED )
SET(CYCLUS_INCLUDE_DIR ${CYCLUS_INCLUDE_DIR} ${COIN_INCLUDE_DIRS})
set(LIBS ${LIBS} ${COIN_LIBRARIES})

##############################################################################################
#################################### end find libraries ######################################
##############################################################################################

##############################################################################################
###################################### begin includes ########################################
##############################################################################################

# include all the directories we just found
INCLUDE_DIRECTORIES( ${CYCLUS_INCLUDE_DIR} )

ADD_SUBDIRECTORY(${CYCLUS_SOURCE_DIR})
ADD_SUBDIRECTORY(${CYCLUS_CLI_DIR})
ADD_SUBDIRECTORY(${CYCLUS_STUB_DIR})
ADD_SUBDIRECTORY(${CYCLUS_TEST_DIR})

##############################################################################################
####################################### end includes #########################################
##############################################################################################

##############################################################################################
################################## begin uninstall target ####################################
##############################################################################################

configure_file(
    "${PROJECT_SOURCE_DIR}/CMake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY
    )

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

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
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "2")
#SET(CPACK_PACKAGE_VERSION_PATCH "2") # should use commit number here  ?
SET(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY "1")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "cyclus${CPACK_VERSION_MAJOR}.${CPACK_VERSION_MINOR}")
#SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-all-dev  (>= 1.46.0-1), libxml++2.6-dev (>= 2.6.0), libsqlite3-dev (>= 3.7.9), libhdf5-serial-dev (>= 1.8.0)")
set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/CMake/CyclusCPackOptions.cmake.in"
  "${CYCLUS_BINARY_DIR}/CMake/CyclusCPackOptions.cmake" @ONLY)
SET(CPACK_PROJECT_CONFIG_FILE "${CYCLUS_BINARY_DIR}/CMake/CyclusCPackOptions.cmake")
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
SET( DOC_SOURCE_DIR ${CYCLUS_SOURCE_DIR} )
SET( DOC_INPUT_DIR ${CYCLUS_DOC_DIR} )
SET( DOC_OUTPUT_DIR ${CMAKE_BINARY_DIR}/doc )
  
# If doxygen exists, use the doc/CMakeLists.txt file for further instructions. 
FIND_PACKAGE(Doxygen)
IF (DOXYGEN_FOUND)
  ADD_SUBDIRECTORY(${CYCLUS_DOC_DIR})
ELSE (DOXYGEN_FOUND)
  MESSAGE(STATUS "WARNING: Doxygen not found - doc won't be created")
ENDIF (DOXYGEN_FOUND)

##############################################################################################
####################################### end doxygen ##########################################
##############################################################################################
