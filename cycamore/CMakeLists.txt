# This project name is cycamore
PROJECT(cycamore)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Direct any out-of-source builds to this directory
SET( CYCAMORE_SOURCE_DIR ${CMAKE_SOURCE_DIR} )

# Direct any binary installation paths to this directory
SET( CYCAMORE_BINARY_DIR ${CMAKE_BINARY_DIR} )

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

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/cycamore/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/cycamore/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/cycamore/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")

# Tell CMake where the modules are
SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_DIR}/share/cmake-2.8/Modules")
SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CYCAMORE_SOURCE_DIR}/CMake )

# Include macros
INCLUDE(CycamoreModuleMacros)

# Debian installs useful LibXML2 files to /usr/include/libxml2/libxml
# KDHFLAG Necessary?
FIND_PACKAGE( LibXml2 REQUIRED )
SET(CYCAMORE_INCLUDE_DIR ${CYCAMORE_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR})
ADD_DEFINITIONS (${LIBXML2_DEFINITIONS})
SET(LIBS ${LIBS} ${LIBXML2_LIBRARIES})

# Find Sqlite3
# KDHFLAG Necessary?
FIND_PACKAGE( Sqlite3 REQUIRED )
SET(CYCAMORE_INCLUDE_DIR ${CYCAMORE_INCLUDE_DIR} ${SQLITE3_INCLUDE_DIR})
SET(LIBS ${LIBS} ${SQLITE3_LIBRARIES})

# Find cycluscore
FIND_PACKAGE( cycluscore REQUIRED )
SET(CYCAMORE_INCLUDE_DIR ${CYCAMORE_INCLUDE_DIR} ${CYCLUS_CORE_INCLUDE_DIR})
SET(LIBS ${LIBS} ${CYCLUS_CORE_LIBRARIES})

# Include the boost header files and the program_options library
# KDHFLAG Necessary?
SET(Boost_USE_STATIC_LIBS       OFF)
SET(Boost_USE_STATIC_RUNTIME    OFF)
FIND_PACKAGE( Boost COMPONENTS program_options filesystem system REQUIRED)
SET(CYCAMORE_INCLUDE_DIR ${CYCAMORE_INCLUDE_DIR} ${BOOST_INCLUDE_DIR})
SET(LIBS ${LIBS} ${Boost_PROGRAM_OPTIONS_LIBRARY})
SET(LIBS ${LIBS} ${Boost_SYSTEM_LIBRARY})
SET(LIBS ${LIBS} ${Boost_FILESYSTEM_LIBRARY})

# include the model directories
SET(CYCAMORE_INCLUDE_DIR ${CYCAMORE_INCLUDE_DIR} Testing )

# include all the directories we just found
INCLUDE_DIRECTORIES( ${CYCAMORE_INCLUDE_DIR} )

# This is the directory that holds the doxygen doxyfile template (doxy.conf.in)
SET( DOC_INPUT_DIR ${CYCAMORE_SOURCE_DIR}/doc)

# If doxygen exists, use the doc/CMakeLists.txt file for further instructions. 
FIND_PACKAGE(Doxygen)
IF (DOXYGEN_FOUND)
  ADD_SUBDIRECTORY(doc)
  SET( DOC_OUTPUT_DIR ${CMAKE_BINARY_DIR}/doc )
ELSE (DOXYGEN_FOUND)
  MESSAGE(STATUS "WARNING: Doxygen not found - doc won't be created")
ENDIF (DOXYGEN_FOUND)

# ------------------------- Add the Models -----------------------------------
ADD_SUBDIRECTORY(Models)

# ------------------------- Google Test -----------------------------------

# Be sure to clear these each time
SET( GENERATED_TEST_LIST "" CACHE INTERNAL "" )
SET( GENERATED_FILTER_LIST "" CACHE INTERNAL "" )
SET( GENERATED_TEST_SOURCE_LIST "" CACHE INTERNAL "" )

# The gtest include file directories.
SET(GTEST_INCLUDE_DIRS "${CYCLUS_INCLUDE_DIRS}")

# The gtest library directories.
SET(GTEST_LIBRARY_DIRS "${CYCLUS_LIBRARY_DIRS}")

# Alerts CMake to any tests we need to worry about
INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})

# Create an executable for all the gtests
# Options for testing
OPTION( USE_TESTING "Build testing" ON )
IF( USE_TESTING )
  enable_testing()
  INCLUDE(CTest)
ENDIF()

ADD_SUBDIRECTORY(Testing)
SET(TestSource 
  ${TestSource} 
  )

FIND_PACKAGE(Threads)
IF(CMAKE_USE_PTHREADS_INIT)  # The pthreads library is available.
  SET(cxx_base_flags "${cxx_base_flags} -DGTEST_HAS_PTHREAD=1")
ENDIF()

# Build CycamoreUnitTestDriver
ADD_EXECUTABLE( CycamoreUnitTestDriver
  Testing/CycamoreUnitTestDriver.cpp ${TestSource}
  ${GENERATED_TEST_SOURCE}
)
TARGET_LINK_LIBRARIES( CycamoreUnitTestDriver dl ${CYCAMORE_LIBRARIES} 
  ${INTERFACE_LIBRARIES} dl ${LIBS} ${CYCLUS_GTEST_LIBRARIES})

# Here we add all the tests
add_google_tests( ${EXECUTABLE_OUTPUT_PATH}/bin/CycamoreUnitTestDriver ${TestSource} 
  ${INTERFACE_LIBRARIES})

INSTALL(TARGETS CycamoreUnitTestDriver
  RUNTIME DESTINATION cycamore/bin
  COMPONENT testing
  )

# Here we set some components for installation with cpack
INSTALL(FILES 
  ${PROJECT_BINARY_DIR}/share/cyclus.rng
  DESTINATION ${CYCLUS_CORE_SHARE_DIR}
  COMPONENT data
  )

SET(CPACK_COMPONENTS_ALL cycamore testing libraries data core)

# Here's where we package it with CPack
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cycamore : The Cyclus Additional Module Repository.")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "CNERG, UW-Madison") #required
SET(CPACK_PACKAGE_VENDOR "CNERG, UW-Madison")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/CMake/README.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/CMake/LICENSE.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
#SET(CPACK_PACKAGE_VERSION_PATCH "2") # should use commit number here  ?
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "cycamore${CPACK_VERSION_MAJOR}.${CPACK_VERSION_MINOR}")
SET(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
 
CONFIGURE_FILE("${CYCAMORE_SOURCE_DIR}/CMake/CycamoreCPackOptions.cmake.in"
  "${CYCAMORE_BINARY_DIR}/CMake/CycamoreCPackOptions.cmake" @ONLY)
SET(CPACK_PROJECT_CONFIG_FILE "${CYCAMORE_BINARY_DIR}/CMake/CycamoreCPackOptions.cmake")
SET(CPACK_PACKAGE_EXECUTABLES "cycamore" "CycamoreUnitTestDriver")


INCLUDE(CPack)


