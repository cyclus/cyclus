# This project name is cyclus.
PROJECT( CYCLUS )
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Direct any out-of-source builds to this directory
SET( CYCLUS_SRC_DIR ${CMAKE_SOURCE_DIR} )

# Includes the directories in which there are source files. 
SET(CYCLUS_INCLUDE_DIR
  ${CYCLUS_SRC_DIR}
  ${CYCLUS_SRC_DIR}/Models
  ${CYCLUS_SRC_DIR}/Utility )

INCLUDE_DIRECTORIES( ${CYCLUS_INCLUDE_DIR} )

# Add any new source files to this list.
SET( CYCLUS_SRC 
     App.cpp 
     InputXML.cpp
     Logician.cpp
     Material.cpp
     Commodity.cpp
     Message.cpp
     Communicator.cpp
     Utility/GenException.cpp 
     Utility/MassTable.cpp 
     Utility/Timer.cpp
     Utility/Env.cpp
     Models/Model.cpp 
        Models/StubModel.cpp 
        Models/StubCommModel.cpp 
        Models/ConverterModel.cpp 
        Models/MarketModel.cpp 
        Models/FacilityModel.cpp 
				Models/RegionModel.cpp
				Models/InstModel.cpp)

set ( CMAKE_MODULE_PATH "${CMAKE_DIR}/share/cmake-2.8/Modules")
set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CYCLUS_SOURCE_DIR}/CMake )


# Copy Data
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/Utility/mass.h5
${CMAKE_CURRENT_BINARY_DIR}/Data/mass.h5
COPYONLY
)
CONFIGURE_FILE(
${CYCLUS_SOURCE_DIR}/cyclus.rng
${CMAKE_CURRENT_BINARY_DIR}/Data/cyclus.rng
COPYONLY
)

# Be sure to clear these each time
set ( GENERATED_TEST_LIST "" CACHE INTERNAL "" )
set ( GENERATED_FILTER_LIST "" CACHE INTERNAL "" )
set ( GENERATED_TEST_SOURCE_LIST "" CACHE INTERNAL "" )

# Add subdirs and build for the models
ADD_SUBDIRECTORY(Models)

# Debian installs useful LibXML2 files to /usr/include/libxml2/libxml
FIND_PACKAGE( LibXml2 REQUIRED )
INCLUDE_DIRECTORIES (${LIBXML2_INCLUDE_DIR})
SET(CYCLUS_INCLUDE_DIR ${CYCLUS_INCLUDE_DIR} ${LIBXML_INCLUDE_DIR})
ADD_DEFINITIONS (${LIBXML2_DEFINITIONS})
set(LIBS ${LIBS} ${LIBXML2_LIBRARIES})

# Include the HDF5 library
FIND_PACKAGE( HDF5 COMPONENTS CXX )
INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIRS})
SET(CYCLUS_INCLUDE_DIR ${CYCLUS_INCLUDE_DIR} ${HDF5_INCLUDE_DIRS})
ADD_DEFINITIONS(${HDF5_DEFINITIONS})
set(LIBS ${LIBS} ${HDF5_CXX_LIBRARIES})

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

# Alerts CMake to any tests we need to worry about
INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})

# Create an executable for all the gtests

# Options for testing
option ( USE_TESTING "Build testing" ON )
if ( USE_TESTING )
  enable_testing()
  subdirs ( Testing )
  include(CTest)
endif()

# Build the cyclus executable from the CYCLUS_SRC source files
ADD_EXECUTABLE( cyclus ${CYCLUS_SRC} )
TARGET_LINK_LIBRARIES( cyclus dl ${LIBS} hdf5 hdf5_hl hdf5_cpp)



