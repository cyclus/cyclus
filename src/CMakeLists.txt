# This project name is cyclus.
PROJECT( CYCLUS )
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Direct any out-of-source builds to this directory
SET( CYCLUS_SRC_DIR ${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES("${CYCLUS_SRC_DIR}")
INCLUDE_DIRECTORIES("${CYCLUS_SRC_DIR}/Models")
INCLUDE_DIRECTORIES("${CYCLUS_SRC_DIR}/Utility")

# Add any new source files to this list.
SET( CYCLUS_SRC 
     App.cpp 
     Logician.cpp
     Material.cpp
     Commodity.cpp
     Utility/GenException.cpp 
     Models/Model.cpp 
        Models/MarketModel.cpp 
        Models/FacilityModel.cpp 
	Models/RegionModel.cpp)

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
TARGET_LINK_LIBRARIES( cyclus dl )
