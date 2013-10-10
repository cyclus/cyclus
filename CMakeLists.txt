# This project name is cyclus.
PROJECT( CYCLUS )
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

IF(NOT CYCLUS_DOC_ONLY)

##############################################################################################
################################# begin cmake configuration ##################################
##############################################################################################

# set project directories
SET( PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET( CYCLUS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src )
SET( CYCLUS_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests )
SET( CYCLUS_STUB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/stubs )

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

# include all the directories we just found
INCLUDE_DIRECTORIES( ${CYCLUS_INCLUDE_DIR} )


##############################################################################################
#################################### end find libraries ######################################
##############################################################################################

##############################################################################################
###################################### begin includes ########################################
##############################################################################################

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(cli)
ADD_SUBDIRECTORY(tests)

##############################################################################################
####################################### end includes #########################################
##############################################################################################

##############################################################################################
###################################### begin testing #########################################
##############################################################################################

##############################################################################################
####################################### end testing ##########################################
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


ENDIF(NOT CYCLUS_DOC_ONLY)  

# add src files