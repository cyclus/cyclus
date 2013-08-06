# This project name is cyclus.
PROJECT( CYCLUS )
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Direct any out-of-source builds to this directory
SET( CYCLUS_SRC_DIR ${CMAKE_SOURCE_DIR} )
  
IF(NOT CYCLUS_DOC_ONLY)
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
SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CYCLUS_SOURCE_DIR}/CMake )

# Include macros
INCLUDE(CyclusModuleMacros)

# Include the core header files in the app build manually.
# Since we build the core library in the same project, this is safer.
SET( CYCLUS_INCLUDE_DIR ${CYCLUS_INCLUDE_DIR}
  ${CYCLUS_SRC_DIR}/Core
  ${CYCLUS_SRC_DIR}/Core/Config
  ${CYCLUS_SRC_DIR}/Core/Resources
  ${CYCLUS_SRC_DIR}/Core/Models
  )

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

# add the core library 
ADD_SUBDIRECTORY(Core)

MESSAGE("libs: ${LIBS}")

# Build the cyclus executable from the CYCLUS_SRC source files
SET( CYCLUS_SRC
  app.cc
  )
ADD_EXECUTABLE( cyclus ${CYCLUS_SRC} )
TARGET_LINK_LIBRARIES( cyclus dl ${LIBS} cycluscore )
INSTALL(TARGETS cyclus
  RUNTIME DESTINATION bin
  COMPONENT cyclus
  )

# ------------------------- Google Test -----------------------------------

# Be sure to clear these each time
SET( GENERATED_TEST_LIST "" CACHE INTERNAL "" )
SET( GENERATED_FILTER_LIST "" CACHE INTERNAL "" )
SET( GENERATED_TEST_SOURCE_LIST "" CACHE INTERNAL "" )

# The gtest include file directories.
SET(GTEST_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Testing/GoogleTest")

# The gtest library directories.
SET(GTEST_LIBRARY_DIRS "${CMAKE_CURRENT_BINARY_DIR}/lib")

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
#  ${CYCLUS_CORE_SRC}
  ${CYCLUS_TEST_CORE}
  )

FIND_PACKAGE(Threads)
IF(CMAKE_USE_PTHREADS_INIT)  # The pthreads library is available.
  SET(cxx_base_flags "${cxx_base_flags} -DGTEST_HAS_PTHREAD=1")
ENDIF()

# Build libgtest
SET( GTest
  Testing/GoogleTest/gtest/gtest-all.cc
)
INCLUDE_DIRECTORIES( Testing )

ADD_LIBRARY(gtest ${GTest})

IF (CMAKE_USE_PTHREADS_INIT)
  TARGET_LINK_LIBRARIES(gtest ${CMAKE_THREAD_LIBS_INIT})
ENDIF()

SET_TARGET_PROPERTIES(gtest
  PROPERTIES INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
  INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
)

INSTALL(TARGETS gtest
  LIBRARY DESTINATION lib
  COMPONENT testing
  )

# Build CyclusUnitTestDriver
ADD_EXECUTABLE( CyclusUnitTestDriver
  Testing/cyclus_unit_test_driver.cc ${TestSource}
  ${GENERATED_TEST_SOURCE}
)
TARGET_LINK_LIBRARIES( CyclusUnitTestDriver dl ${LIBS} cycluscore gtest)

# Here we add all the tests
add_google_tests( ${EXECUTABLE_OUTPUT_PATH}/bin/CyclusUnitTestDriver 
  ${TestSource}) 

INSTALL(TARGETS CyclusUnitTestDriver
  RUNTIME DESTINATION bin
  COMPONENT testing
  )

# Here we set some components for installation with cpack
INSTALL(FILES 
  ${PROJECT_BINARY_DIR}/share/cyclus.rng
  ${PROJECT_BINARY_DIR}/share/cyclus.rng.in
  DESTINATION share
  COMPONENT data
  )

SET(CPACK_COMPONENTS_ALL cyclus testing libraries data core)

# Here's where we package it with CPack
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cyclus : A nuclear fuel cycle simulator from UW-Madison.")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "CNERG, UW-Madison") #required
SET(CPACK_PACKAGE_VENDOR "CNERG, UW-Madison")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Core/Config/README.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Core/Config/LICENSE.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
#SET(CPACK_PACKAGE_VERSION_PATCH "2") # should use commit number here  ?
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "cyclus${CPACK_VERSION_MAJOR}.${CPACK_VERSION_MINOR}")
SET(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")

CONFIGURE_FILE("${CYCLUS_SOURCE_DIR}/Core/Config/CyclusCPackOptions.cmake.in"
  "${CYCLUS_BINARY_DIR}/Core/Config/CyclusCPackOptions.cmake" @ONLY)
SET(CPACK_PROJECT_CONFIG_FILE "${CYCLUS_BINARY_DIR}/Core/Config/CyclusCPackOptions.cmake")
SET(CPACK_PACKAGE_EXECUTABLES "cyclus" "CyclusUnitTestDriver")


INCLUDE(CPack)

ENDIF(NOT CYCLUS_DOC_ONLY)  

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

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/Config/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
