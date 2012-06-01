# This project name is cyclus.
PROJECT( CYCLUS )
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Direct any out-of-source builds to this directory
SET( CYCLUS_SRC_DIR ${CMAKE_SOURCE_DIR} )

# # Determine the output directory
# IF (NOT EXISTS ENV{CYCLUS_OUT_DIR})
#   SET( ENV{CYCLUS_OUT_DIR} ${CMAKE_CURRENT_BINARY_DIR} )
#   MESSAGE(STATUS "CYCLUS_OUT_DIR not found, setting to ${CMAKE_CURRENT_BINARY_DIR}")
# ENDIF(NOT EXISTS ENV{CYCLUS_OUT_DIR})

# This makes all the libraries build as SHARED
SET(BUILD_SHARED_LIBS true)

# This allows apple to build the libraries dynamically
IF (WIN32)
  set(suffix ".dll")
  set(dynamicloadlib "WinLoadConstr.h")
  CONFIGURE_FILE(${CYCLUS_SRC_DIR}/Config/suffix.h.in ${CYCLUS_SRC_DIR}/Config/suffix.h @ONLY)
ELSEIF(APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup")
  set(suffix ".dylib")
  set(dynamicloadlib "UnixLoadConstr.h")
  CONFIGURE_FILE(${CYCLUS_SRC_DIR}/Config/suffix.h.in ${CYCLUS_SRC_DIR}/Config/suffix.h @ONLY)
ELSE()
  set(suffix ".so")
  set(dynamicloadlib "UnixLoadConstr.h")
  CONFIGURE_FILE(${CYCLUS_SRC_DIR}/Config/suffix.h.in ${CYCLUS_SRC_DIR}/Config/suffix.h @ONLY)
ENDIF()

# Copy Data
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Utility/mass.sqlite
${CMAKE_CURRENT_BINARY_DIR}/Data/mass.sqlite
COPYONLY
)

CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Config/cyclus.rng.in
${PROJECT_BINARY_DIR}/Data/cyclus.rng
COPYONLY
)

CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Utility/decayInfo.dat
${CMAKE_CURRENT_BINARY_DIR}/Data/decayInfo.dat
COPYONLY
)

set(input_path ${CYCLUS_SOURCE_DIR}/../input)
set(cyclus_path ${CMAKE_CURRENT_BINARY_DIR})
CONFIGURE_FILE(${CYCLUS_SRC_DIR}/Config/run_inputs.py.in ${PROJECT_BINARY_DIR}/Testing/run_inputs.py @ONLY)

# Setup build locations.
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CYCLUS_BINARY_DIR}/bin)
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CYCLUS_BINARY_DIR}/lib)
endif()
#if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
#  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CYCLUS_BINARY_DIR}/lib)
#endif()


# ----- Cyclus Unit Test Driver ------

# Includes the directories in which there are source files. 
SET(CYCLUS_INCLUDE_DIR
  ${CYCLUS_SRC_DIR}
  ${CYCLUS_SRC_DIR}/Config
  ${CYCLUS_SRC_DIR}/Input
  ${CYCLUS_SRC_DIR}/Models
  ${CYCLUS_SRC_DIR}/Resources
  ${CYCLUS_SRC_DIR}/Utility
  )


# Add any new core source files to this list.
SET( CYCLUS_CORE
  Models/Model.cpp 
  Models/Message.cpp
  Models/Transaction.cpp
  Models/StubTimeAgent.cpp 
  Models/StubModel.cpp 
  Models/StubCommModel.cpp 
  Models/ConverterModel.cpp 
  Models/MarketModel.cpp 
  Models/FacilityModel.cpp 
  Models/RegionModel.cpp
  Models/InstModel.cpp
  )

ADD_SUBDIRECTORY(Input)
ADD_SUBDIRECTORY(Resources)
ADD_SUBDIRECTORY(Utility)

SET( CYCLUS_SRC
  ${CYCLUS_CORE}
  App.cpp
  )

set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_DIR}/share/cmake-2.8/Modules")
set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CYCLUS_SOURCE_DIR}/CMake )

# Debian installs useful LibXML2 files to /usr/include/libxml2/libxml
FIND_PACKAGE( LibXml2 REQUIRED )
SET(CYCLUS_INCLUDE_DIR ${CYCLUS_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR})
ADD_DEFINITIONS (${LIBXML2_DEFINITIONS})
set(LIBS ${LIBS} ${LIBXML2_LIBRARIES})

# Find Sqlite3
FIND_PACKAGE( Sqlite3 REQUIRED )
SET(CYCLUS_INCLUDE_DIR ${CYCLUS_INCLUDE_DIR} ${SQLITE3_INCLUDE_DIR})
set(LIBS ${LIBS} ${SQLITE3_LIBRARIES})

# Include the boost header files and the program_options library
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
FIND_PACKAGE( Boost COMPONENTS program_options filesystem system REQUIRED)
SET(CYCLUS_INCLUDE_DIR ${CYCLUS_INCLUDE_DIR} ${BOOST_INCLUDE_DIR})
set(LIBS ${LIBS} ${Boost_PROGRAM_OPTIONS_LIBRARY})
set(LIBS ${LIBS} ${Boost_SYSTEM_LIBRARY})
set(LIBS ${LIBS} ${Boost_FILESYSTEM_LIBRARY})

# include all the directories we just found
INCLUDE_DIRECTORIES( ${CYCLUS_INCLUDE_DIR} )

# Add subdirs and build for the models
ADD_SUBDIRECTORY(Models)

# This is the directory that holds the doxygen doxyfile template (doxy.conf.in)
SET( DOC_INPUT_DIR ${CYCLUS_SRC_DIR}/doc)

# If doxygen exists, use the doc/CMakeLists.txt file for further instructions. 
FIND_PACKAGE(Doxygen)
IF (DOXYGEN_FOUND)
	ADD_SUBDIRECTORY(doc)
	SET( DOC_OUTPUT_DIR ${CMAKE_BINARY_DIR}/doc )
ELSE (DOXYGEN_FOUND)
	MESSAGE(STATUS "WARNING: Doxygen not found - doc won't be created")
ENDIF (DOXYGEN_FOUND)

# Build the cyclus executable from the CYCLUS_SRC source files
ADD_EXECUTABLE( cyclus ${CYCLUS_SRC} )
TARGET_LINK_LIBRARIES( cyclus dl ${LIBS} )


# Be sure to clear these each time
set ( GENERATED_TEST_LIST "" CACHE INTERNAL "" )
set ( GENERATED_FILTER_LIST "" CACHE INTERNAL "" )
set ( GENERATED_TEST_SOURCE_LIST "" CACHE INTERNAL "" )

# The gtest include file directories.
SET(GTEST_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Testing/GoogleTest")

# The gtest library directories.
SET(GTEST_LIBRARY_DIRS "${CMAKE_CURRENT_BINARY_DIR}/Testing")

# Alerts CMake to any tests we need to worry about
INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})

# Create an executable for all the gtests
# Options for testing
option ( USE_TESTING "Build testing" ON )
if ( USE_TESTING )
  enable_testing()
  include(CTest)
endif()

ADD_SUBDIRECTORY(Testing)
SET(TestSource 
  ${TestSource} 
  ${CYCLUS_CORE}
  ${CYCLUS_TEST_CORE}
  )

# Build Google Test
find_package(Threads)
if (CMAKE_USE_PTHREADS_INIT)  # The pthreads library is available.
  set(cxx_base_flags "${cxx_base_flags} -DGTEST_HAS_PTHREAD=1")
endif()

# Build Google Testing
set ( GTest
  Testing/GoogleTest/gtest/gtest-all.cc
)
include_directories ( Testing ) # necessary?

add_library(gtest ${GTest})
if (CMAKE_USE_PTHREADS_INIT)
  target_link_libraries(gtest ${CMAKE_THREAD_LIBS_INIT})
endif()

INCLUDE_DIRECTORIES(Testing/Googletest) # necessary?
add_executable( CyclusUnitTestDriver
  Testing/CyclusUnitTestDriver.cpp ${TestSource}
  ${GENERATED_TEST_SOURCE}
)

target_link_libraries ( CyclusUnitTestDriver dl ${CYCLUS_LIBRARIES} 
  ${INTERFACE_LIBRARIES} dl ${LIBS} gtest)

# Here we add all the tests
add_google_tests ( ${EXECUTABLE_OUTPUT_PATH}/CyclusUnitTestDriver ${TestSource} 
  ${INTERFACE_LIBRARIES})

# Here we set some components for installation with cpack
 install(TARGETS 
     ${INTERFACE_LIBRARIES} 
   ARCHIVE
   DESTINATION cyclus/lib
   COMPONENT libraries
   )
 install(TARGETS 
     cyclus
   RUNTIME
   DESTINATION cyclus/bin
   COMPONENT application
   )
 install(TARGETS 
     CyclusUnitTestDriver
   RUNTIME
   DESTINATION cyclus/bin
   COMPONENT application
   )
 install(FILES 
     Input/cyclus.rng
     Utility/mass.sqlite
     Utility/decayInfo.dat
   DESTINATION  
     cyclus/Data
   COMPONENT data
   )

set(CPACK_COMPONENTS_ALL cyclus libraries data)

# Here's where we package it with CPack
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cyclus : A nuclear fuel cycle simulator from UW-Madison.")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "CNERG, UW-Madison") #required
SET(CPACK_PACKAGE_VENDOR "CNERG, UW-Madison")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Config/README.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Config/LICENSE.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
#SET(CPACK_PACKAGE_VERSION_PATCH "2") # should use commit number here  ?
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "cyclus${CPACK_VERSION_MAJOR}.${CPACK_VERSION_MINOR}")
SET(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
 
CONFIGURE_FILE("${CYCLUS_SOURCE_DIR}/Config/CyclusCPackOptions.cmake.in"
  "${CYCLUS_BINARY_DIR}/Config/CyclusCPackOptions.cmake" @ONLY)
SET(CPACK_PROJECT_CONFIG_FILE "${CYCLUS_BINARY_DIR}/Config/CyclusCPackOptions.cmake")
SET(CPACK_PACKAGE_EXECUTABLES "cyclus" "CyclusUnitTestDriver")


INCLUDE(CPack)
 
